/*
 * Copyright 2021-2022 Thingstream AG
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
 * @brief UDP configurations for supported Thales modems
 * @{
 */

#ifndef INC_THALES_MODEM_CONFIG_H
#define INC_THALES_MODEM_CONFIG_H

#include "modem_transport.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif


/**
 * If the SDK sees ^SYSSTART during the init sequence then the SDK will
 * delay for #Thingstream_ThalesExs_initDelayWithSysstart milliseconds before
 * attempting to activate the networking. This extra delay avoids problems
 * that we have seen with some devices and mobile operators.
 *
 * The SDK is built with this value defaulting to 5000.
 *
 * If your application needs to change this value then it may do so by adding
 * the following code snippet to the application:
 *
 *   const uint32_t Thingstream_ThalesExs_initDelayWithSysstart = newValue;
 */
extern const uint32_t Thingstream_ThalesExs_initDelayWithSysstart;

/**
 * If the SDK does not see ^SYSSTART during the init sequence then the SDK will
 * delay for #Thingstream_ThalesExs_initDelayWithoutSysstart milliseconds before attempting to
 * activate the networking. The SDK is built with this value defaulting to 0
 * but if your application needs to change this value then it may do so by
 * adding the following code snippet to the application:
 *
 *   const uint32_t Thingstream_ThalesExs_initDelayWithoutSysstart = newValue;
 */
extern const uint32_t Thingstream_ThalesExs_initDelayWithoutSysstart;


/**
 * @addtogroup udp_modems
 * @{
 */

/**
 * Pass #Thingstream_ThalesExs62wInit to Thingstream_createModemTransport()
 * to configure the modem driver to match the Thales Exs62-w hardware.
 */
extern ThingstreamModemUdpInit Thingstream_ThalesExs62wInit;


/**
 * Pass #Thingstream_ThalesExs82wInit to Thingstream_createModemTransport()
 * to configure the modem driver to match the Thales Exs82-w hardware.
 */
extern ThingstreamModemUdpInit Thingstream_ThalesExs82wInit;

/** @} */

#if defined(__cplusplus)
}
#endif

#endif /* INC_THALES_MODEM_CONFIG_H */
/** @} */
