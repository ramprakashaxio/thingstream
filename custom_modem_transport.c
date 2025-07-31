/*
 * Copyright 2017-2023 Thingstream AG
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
 * @brief ThingstreamTransport template for communicating with a custom modem
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <custom_modem_transport.h>
#include <transport_api.h>

#include <sdk_data.h>

#ifdef USE_CUSTOM_MODEM_DRIVER

/**
 * Set the 'receive message' buffer size according to the protocol.
 * Suggest THINGSTREAM_UDP_BUFFER_LEN for UDP and
 * THINGSTREAM_USSD_BUFFER_LEN for USSD.
 */
#define BUFFER_SIZE THINGSTREAM_UDP_BUFFER_LEN

#ifndef THINGSTREAM_SHORTCODE
/**
 * The THINGSTREAM_SHORTCODE is the USSD shortcode required to communicate
 * with the Thingstream USSD server.
 */
#define THINGSTREAM_SHORTCODE             "469"
#endif /* THINGSTREAM_SHORTCODE */

/**
 * The CustomModemState structure is used to store state for the custom modem
 * transport.
 */
typedef struct
{
    /**
     * Store the registered 'callback'.
     * This will be invoked when this transport has data to send up the
     * Thingstream stack.
     */
    ThingstreamTransportCallback_t callback;
    /**
     * Cookie associated with the registered callback.
     */
    void* cookie;

    /**
     * The buffer used to receive inbound messages.
     */
    uint8_t buffer[BUFFER_SIZE];
} CustomModemState;

/** Instance of CustomModemState */
static CustomModemState _custom_modem_transport_state;

static ThingstreamTransportResult custom_modem_init(ThingstreamTransport* self, uint16_t version);
static ThingstreamTransportResult custom_modem_shutdown(ThingstreamTransport* self);
static ThingstreamTransportResult custom_modem_get_buffer(ThingstreamTransport* self, uint8_t** buffer, uint16_t* len);
static ThingstreamTransportResult custom_modem_send(ThingstreamTransport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t millis);
static ThingstreamTransportResult custom_modem_register_transport_callback(ThingstreamTransport* self, ThingstreamTransportCallback_t callback, void* cookie);
static ThingstreamTransportResult custom_modem_run(ThingstreamTransport* self, uint32_t millis);

/** Instance of ThingstreamTransport for this custom transport */
static const ThingstreamTransport _custom_modem_transport_instance = {
    (ThingstreamTransportState_t*)&_custom_modem_transport_state,
    custom_modem_init,
    custom_modem_shutdown,
    custom_modem_get_buffer,
    NULL,
    custom_modem_send,
    custom_modem_register_transport_callback,
    NULL,
    custom_modem_run
};

/**
 * Create an instance of the modem transport.
 *
 * params porting specific options
 * @return the instance
 */
ThingstreamTransport* Thingstream_createCustom_modemTransport(/* porting specific options */)
{
    ThingstreamTransport *self = (ThingstreamTransport*)&_custom_modem_transport_instance;
    CustomModemState* state = (CustomModemState*)self->_state;

    /* TODO: save any porting specific options in state->xxx */

    return self;
}

/**
 * Initialize the transport.
 * This may involve the setup on GPIO, UART ports, interrupts and other
 * platform dependencies.
 *
 * @param self the #ThingstreamTransport instance of this custom modem
 * @param version the transport API version
 * @return a #ThingstreamTransportResult status code (success / fail)
 */
static ThingstreamTransportResult custom_modem_init(ThingstreamTransport* self, uint16_t version)
{
    ThingstreamTransportResult tRes = TRANSPORT_SUCCESS;
    CustomModemState* state = (CustomModemState*)self->_state;

    if (!TRANSPORT_CHECK_VERSION_1(version))
    {
        return TRANSPORT_VERSION_MISMATCH;
    }

    state->callback = NULL;

    /* TODO: fill in initialisation details */

    return tRes;
}

/**
 * Shutdown the transport (i.e. the opposite of initialize)
 *
 * @param self the #ThingstreamTransport instance of this custom modem
 * @return a #ThingstreamTransportResult status code (success / fail)
 */
static ThingstreamTransportResult custom_modem_shutdown(ThingstreamTransport* self)
{
    ThingstreamTransportResult tRes = TRANSPORT_SUCCESS;
    CustomModemState* state = (CustomModemState*)self->_state;

    state->callback = NULL;

    /* TODO:
     * Undo any initialisations performed when custom_modem_init() was called.
     */

    return tRes;
}

/**
 * Provide details of this transport's 'receive buffer'.
 *
 * @param self the #ThingstreamTransport instance of this custom modem
 * @param buffer where to write the 'receive buffer' pointer
 * @param len where to the write the 'receive buffer' length
 * @return a #ThingstreamTransportResult status code (success / fail)
 */
static ThingstreamTransportResult custom_modem_get_buffer(ThingstreamTransport* self, uint8_t** buffer, uint16_t* len)
{
    CustomModemState* state = (CustomModemState*)self->_state;
    *buffer = state->buffer;
    *len = sizeof(state->buffer);
    return TRANSPORT_SUCCESS;
}

/**
 * Send data to the Thingstream server.
 *
 * *USSD only*: using #THINGSTREAM_SHORTCODE\n
 * *USSD only*: if 'flags' contains the #TSEND_USSD_SESSION_END bit
 *   terminate the session after sending the payload
 *
 * @param self the #ThingstreamTransport instance of this custom modem
 * @param flags an indication of the type of the data, zero is normal.
 * @param data a pointer to the data
 * @param len the length of the raw data
 * @param millis the maximum number of milliseconds to run
 * @return a #ThingstreamTransportResult status code (success / fail)
 */
static ThingstreamTransportResult custom_modem_send(ThingstreamTransport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t millis)
{
    ThingstreamTransportResult tRes = TRANSPORT_SUCCESS;
    CustomModemState* state = (CustomModemState*)self->_state;

    /* TODO:
     * Send the 'data' bytes to the Thingstream server.
     */

    return tRes;
}

/**
 * Register a callback function that will be called when this transport
 * has data to send to its next outermost ThingstreamTransport.
 *
 * @param self the #ThingstreamTransport instance of this custom modem
 * @param callback the callback function
 * @param cookie a opaque value passed to the callback function
 * @return a #ThingstreamTransportResult status code (success / fail)
 */
static ThingstreamTransportResult custom_modem_register_transport_callback(ThingstreamTransport* self, ThingstreamTransportCallback_t callback, void* cookie)
{
    CustomModemState* state = (CustomModemState*)self->_state;
    state->callback = callback;
    state->cookie = cookie;
    return TRANSPORT_SUCCESS;
}

/**
 * Allow the transport instance to run for at most the given number of
 * milliseconds. Check for any incoming data and pass it up the
 * Thingstream transport stack using the previously registered callback.
 * The callback must not be called unless custom_modem_run() is being
 * called by the Thingstream transport stack.
 *
 * @param self the #ThingstreamTransport instance of this custom modem
 * @param millis the maximum number of milliseconds to run (a value of zero
 *        processes all pending operations).
 * @return a #ThingstreamTransportResult status code (success / fail)
 */
static ThingstreamTransportResult custom_modem_run(ThingstreamTransport* self, uint32_t millis)
{
    ThingstreamTransportResult tRes = TRANSPORT_SUCCESS;
    CustomModemState* state = (CustomModemState*)self->_state;

    /* TODO:
     *
     * Send any received data up the Thingstream transport stack
     * via the registered callback.
     *
     * Unexpected "incoming" errors can be reported by returning a
     * #ThingstreamTransportResult error code.
     */

    return tRes;
}

#endif /* USE_CUSTOM_MODEM_DRIVER */
