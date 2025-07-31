/*
 * Copyright 2019-2025 Thingstream AG
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
 * @brief UDP configurations for supported SimCom and related modems
 */

/**
 * @addtogroup udp_modems
 * @{
 */

#ifndef INC_SIMCOM_MODEM_CONFIG_H
#define INC_SIMCOM_MODEM_CONFIG_H

#include "modem_transport.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * Pass #Thingstream_Simcom800Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the Simcom SIM800 hardware.
 */
extern ThingstreamModemUdpInit Thingstream_Simcom800Init;

/**
 * Pass #Thingstream_Simcom868Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the Simcom SIM868 hardware.
 */
extern ThingstreamModemUdpInit Thingstream_Simcom868Init;

/**
 * Pass #Thingstream_Simcom7000Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the Simcom SIM7000 hardware.
 */
extern ThingstreamModemUdpInit Thingstream_Simcom7000Init;

/**
 * Pass #Thingstream_Simcom7070Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the Simcom SIM7070 hardware.
 */
extern ThingstreamModemUdpInit Thingstream_Simcom7070Init;

/**
 * Pass #Thingstream_Simcom7080Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the Simcom SIM7080 hardware.
 */
extern ThingstreamModemUdpInit Thingstream_Simcom7080Init;

/**
 * Pass #Thingstream_Simcom7600Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the Simcom SIM7600 hardware.
 */
extern ThingstreamModemUdpInit Thingstream_Simcom7600Init;

/**
 * Pass #Thingstream_LynqL511Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the Lynq L511 hardware.
 */
extern ThingstreamModemUdpInit Thingstream_LynqL511Init;

/**
 * Pass #Thingstream_CavliC16QSInit to Thingstream_createModemTransport()
 * to configure the modem driver to match the Cavli C16QS hardware.
 */
extern ThingstreamModemUdpInit Thingstream_CavliC16QSInit;

#if defined(__cplusplus)
}
#endif

#endif /* INC_SIMCOM_MODEM_CONFIG_H */
/** @} */
