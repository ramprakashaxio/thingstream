/*
 * Copyright 2021 Thingstream AG
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
 */
#ifndef INC_SERIAL_TRANSPORT_H_
#define INC_SERIAL_TRANSPORT_H_


#include <stdint.h>

#include "nrf_drv_uart.h"
#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif


/**
 * Create a Serial instance that transfers bytes over a serial port.
 * @param p_comm_config a pointer to configuration for the UART
 * @return an instance of Serial
 */
extern ThingstreamTransport* serial_transport_create(nrf_drv_uart_config_t *p_comm_config);

#if defined(__cplusplus)
}
#endif

#endif /* INC_SERIAL_TRANSPORT_H_ */
