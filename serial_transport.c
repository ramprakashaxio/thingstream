/*
 * Copyright 2021-2023 Thingstream AG
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 * @brief An interface to communicate over a serial port.
 * This code defines a set of serial transport routines to support
 * the Thingstream SDK, see `transport_api.h` for more details.
 */

#include <stdbool.h>
#include <string.h>
#include "serial_transport.h"
#include "client_platform.h"
#include "nrf_drv_uart.h"

/* Assume an instance number of zero (same as APP_UART_DRIVER_INSTANCE) */
#define TS_UART_DRIVER_INSTANCE   0

/* We need a small buffer to copy any serial_send() data that is in flash.
 * The buffer does not need to be very big since the only SDK data that is
 * sent from flash is small, fixed, AT+XXXX commands.
 */
#define MAX_TX_BUFFER      (64)

/**
 * This is the Serial transport state which records callback details,
 * receive and transmit buffers and flags.
 */
typedef struct SerialTransportState_s {
    ThingstreamTransportCallback_t callback;
    void* callback_cookie;
    nrf_drv_uart_t uart;
    uint8_t rx_buffer[1];
    uint8_t tx_buffer[MAX_TX_BUFFER];
    volatile bool tx_busy;
} SerialTransportState;


static SerialTransportState _transport_state;

static ThingstreamTransportResult serial_init(ThingstreamTransport* self, uint16_t version);
static ThingstreamTransportResult serial_shutdown(ThingstreamTransport* self);
static ThingstreamTransportResult serial_send(ThingstreamTransport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t millis);
static ThingstreamTransportResult serial_register_callback(ThingstreamTransport* self, ThingstreamTransportCallback_t callback, void* cookie);
static ThingstreamTransportResult serial_run(ThingstreamTransport* self, uint32_t millis);

static const ThingstreamTransport _transport_instance = {
    (ThingstreamTransportState_t*)&_transport_state,
    serial_init,
    serial_shutdown,
    NULL, /* get_buffer()     not used with ring buffer transport */
    NULL, /* This slot no longer used */
    serial_send,
    serial_register_callback,
    NULL, /* This slot no longer used */
    serial_run
};

static void uart_event_handler(nrf_drv_uart_event_t * p_event, void* p_context)
{
    SerialTransportState *state = (SerialTransportState*)p_context;

    if (p_event->type == NRF_DRV_UART_EVT_RX_DONE)
    {
        // Received bytes counter has to be checked, because there could be event from RXTO interrupt
        if (p_event->data.rxtx.bytes)
        {
            ThingstreamTransportCallback_t callback = state->callback;
            if (callback != NULL)
            {
                callback(state->callback_cookie,
                         p_event->data.rxtx.p_data,
                         p_event->data.rxtx.bytes);
            }
        }
        (void)nrf_drv_uart_rx(&state->uart, state->rx_buffer, sizeof(state->rx_buffer));
    }
    else if (p_event->type == NRF_DRV_UART_EVT_TX_DONE)
    {
        state->tx_busy = false;
    }
    else if (p_event->type == NRF_DRV_UART_EVT_ERROR)
    {
        (void)nrf_drv_uart_rx(&state->uart, state->rx_buffer, sizeof(state->rx_buffer));
    }
}

/**
 * Create a serial ThingstreamTransport instance that transfers bytes
 * over a serial port.
 * @param p_comm_config a pointer to configuration for the UART
 * @return an instance of serial ThingstreamTransport
 */
ThingstreamTransport* serial_transport_create(nrf_drv_uart_config_t *p_comm_config)
{
    ThingstreamTransport *self = (ThingstreamTransport*)&_transport_instance;
    SerialTransportState *state = (SerialTransportState*)self->_state;

    const static nrf_drv_uart_t uart_conf = NRF_DRV_UART_INSTANCE(TS_UART_DRIVER_INSTANCE);
    state->uart = uart_conf;
    nrf_drv_uart_config_t config = *p_comm_config;
    config.p_context = state;
    uint32_t nRet = nrf_drv_uart_init(&state->uart, &config, uart_event_handler);
    if (nRet != NRF_SUCCESS)
        return NULL;
    else
        return self;
}

/**
 * Initialize the serial transport.
 * For instance, setup GPIO, UART ports, interrupts.
 * @param version the transport API version
 * @return integer status code (success / fail)
 */
static ThingstreamTransportResult serial_init(ThingstreamTransport* self, uint16_t version)
{
    SerialTransportState* state = (SerialTransportState*)self->_state;
    if (!TRANSPORT_CHECK_VERSION_1(version))
    {
        return TRANSPORT_VERSION_MISMATCH;
    }

    /* Activate the UART receiver, bytes will be delivered via our callback */
    ret_code_t nRet = nrf_drv_uart_rx(&state->uart, state->rx_buffer, sizeof(state->rx_buffer));
    if (nRet == NRF_SUCCESS)
        return TRANSPORT_SUCCESS;
    else
        return TRANSPORT_ERROR;
}

/**
 * Shutdown the serial transport (the opposite of initialize)
 * @return an integer status code (success / fail)
 */
static ThingstreamTransportResult serial_shutdown(ThingstreamTransport* self)
{
    SerialTransportState* state = (SerialTransportState*)self->_state;
    nrf_drv_uart_uninit(&state->uart);
    return TRANSPORT_SUCCESS;
}

/**
 * Send the data to the serial device.
 *
 * @param flags an indication of the type of the data, zero is normal.
 * @param data a pointer to the data
 * @param len the length of the raw data
 * @param millis the maximum number of milliseconds to run
 * @return an integer status code (success / fail)
 */
static ThingstreamTransportResult serial_send(ThingstreamTransport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t millis)
{
    SerialTransportState* state = (SerialTransportState*)self->_state;

    uint32_t now = Thingstream_Platform_getTimeMillis();
    uint32_t limit = now + millis;

    /* We can't use Nordic blocking TX since we need RX callback events.
     * But the UARTE tx routine will not allow writes from data in flash.
     * Copy any such arguments to a local buffer (they are relatively short
     * fixed AT commands).
     */
    if (!nrfx_is_in_ram(data))
    {
        if (len > sizeof(state->tx_buffer))
        {
            return TRANSPORT_ILLEGAL_ARGUMENT;
        }
        memcpy(state->tx_buffer, data, len);
        data = state->tx_buffer;
    }

    state->tx_busy = 1;

    /* We don't use nrf_drv_uart_tx() here because its length parameter is
     * only uint8_t. Instead this is a manual inline of the code.
     */
    ret_code_t nRet;
    nrf_drv_uart_t *p_instance = &state->uart;
    if (NRF_DRV_UART_USE_UARTE)
    {
        nRet = nrfx_uarte_tx(&p_instance->uarte, data, len);
    }
    else if (NRF_DRV_UART_USE_UART)
    {
        nRet = nrfx_uart_tx(&p_instance->uart, data, len);
    }

    if (nRet != NRF_SUCCESS)
    {
        return TRANSPORT_ERROR;
    }

    while (state->tx_busy)
    {
        __WFI();
        if (state->tx_busy && TIME_COMPARE(Thingstream_Platform_getTimeMillis(), >, limit))
        {
            nrf_drv_uart_tx_abort(&state->uart);
            return TRANSPORT_SEND_TIMEOUT;
        }
    }

    return TRANSPORT_SUCCESS;
}

/**
 * Register a callback function that will be called when this transport
 * has data to send to its next outermost ThingstreamTransport.
 *
 * @param callback the callback function
 * @param cookie a opaque value passed to the callback function
 * @return an integer status code (success / fail)
 */
static ThingstreamTransportResult serial_register_callback(ThingstreamTransport* self, ThingstreamTransportCallback_t callback, void* cookie)
{
    SerialTransportState* state = (SerialTransportState*)self->_state;
    state->callback = callback;
    state->callback_cookie = cookie;
    return TRANSPORT_SUCCESS;
}

/**
 * Allow the serial transport instance to run for at most the given
 * number of milliseconds.
 * @param millis the maximum number of milliseconds to run
 *        (a value of zero processes all pending operations).
 * @return an integer status code (success / fail)
 */
static ThingstreamTransportResult serial_run(ThingstreamTransport* self, uint32_t millis)
{
    /* The Thingstream stack will wait for modem responses by repeatedly calling
     * serial_run() so this is a convenient place to notify watchdog timers
     * or to reduce power consumption by waiting for the next interrupt.
     * We have no work to do here since our UART data is sent to the
     * Thingstream SDK via callbacks.
     */
    __WFI();
    return TRANSPORT_SUCCESS;
}
