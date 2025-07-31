/*
 * Copyright 2019-2024 Thingstream AG
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
 * @brief UDP configurations for supported u-blox modems
 * @addtogroup udp_modems
 * @{
 */

#ifndef INC_UBLOX_MODEM_CONFIG_H
#define INC_UBLOX_MODEM_CONFIG_H

#include "modem_transport.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * Pass #Thingstream_uBloxLaraR2Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox LARA-R2xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxLaraR2Init;

/**
 * Pass #Thingstream_uBloxLaraR6Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox LARA-R6xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxLaraR6Init;

/**
 * Pass #Thingstream_uBloxLenaR8Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox LENA-R8xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxLenaR8Init;

/**
 * Pass #Thingstream_uBloxLexiR4Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox LEXI-R4xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxLexiR4Init;

/**
 * Pass #Thingstream_uBloxLexiR5Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox LEXI-R5xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxLexiR5Init;

/**
 * Pass #Thingstream_uBloxLexiR10Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox LEXI-R10xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxLexiR10Init;

/**
 * Pass #Thingstream_uBloxSaraG350Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox SARA-G350 hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxSaraG350Init;

/**
 * Pass #Thingstream_uBloxSaraG450Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox SARA-G450 hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxSaraG450Init;

/**
 * Pass #Thingstream_uBloxSaraR4Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox SARA-R4xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxSaraR4Init;

/**
 * Pass #Thingstream_uBloxSaraR5Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox SARA-R5xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxSaraR5Init;

/**
 * Pass #Thingstream_uBloxSaraR10Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox SARA-R10xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxSaraR10Init;

/**
 * Pass #Thingstream_uBloxSaraU2Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox SARA-U2xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxSaraU2Init;

/**
 * Pass #Thingstream_uBloxTobyR2Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox TOBY-R2xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxTobyR2Init;

/**
 * Pass #Thingstream_uBloxTobyL2Init to Thingstream_createModemTransport()
 * to configure the modem driver to match the u-blox TOBY-L2xx hardware.
 */
extern ThingstreamModemUdpInit Thingstream_uBloxTobyL2Init;

#if defined(__cplusplus)
}
#endif

#endif /* INC_UBLOX_MODEM_CONFIG_H */
/** @} */
