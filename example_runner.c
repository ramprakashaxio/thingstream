/*
 * Copyright 2019-2022 Thingstream AG
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

#include <string.h>

#include "pca10056.h"

#include "application.h"
#include "run_example.h"

#include "serial_transport.h"

/** Optional modem flags to pass to Thingstream_createModemTransport() */
static uint32_t modem_flags;

/*
 * Run Thingstream example.
 */
void runApplication()
{
    Thingstream_Util_printf("Thingstream example application starting\n");

    nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
    config.pselrxd = ARDUINO_0_PIN;
    config.pseltxd = ARDUINO_1_PIN;
    config.hwfc = false;
#if defined (UART_PRESENT)
    config.baudrate = (nrf_uart_baudrate_t)NRF_UART_BAUDRATE_115200;
#else
    config.baudrate = (nrf_uarte_baudrate_t)NRF_UARTE_BAUDRATE_115200;
#endif

    ThingstreamTransport* transport = serial_transport_create(&config);
    if (transport == NULL)
    {
        Thingstream_Util_printf("serial creation failed\n");
    }
    else
    {
        (void)run_example(transport, UDP_MODEM_INIT, modem_flags);
    }
}
