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
 * @brief The deprecated previous file name for modem_transport.h
 */

#ifndef INC_MODEM2_TRANSPORT_H
#define INC_MODEM2_TRANSPORT_H

#include "modem_transport.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * The 'logger' parameter is no longer used.
 *
 * @deprecated Alternative Thingstream_createModemTransport() is preferred
 */
#define Thingstream_createModem2Transport(inner,flags,buffer,bufSize,udpConfigInit,logger) \
    Thingstream_createModemTransport(inner,flags,buffer,bufSize,udpConfigInit)

/** @deprecated renamed to Thingstream_Modem_sendLine() */
extern ThingstreamTransportResult Thingstream_Modem2_sendLine(ThingstreamTransport* self, const char* line, uint32_t millis);

/** @deprecated renamed to Thingstream_Modem_getCUSDErrors() */
#define Thingstream_Modem2_getCUSDErrors(self, andClear) \
        Thingstream_Modem_getCUSDErrors((self), (andClear))

/** @deprecated renamed to Thingstream_Modem_getSeriousErrors() */
#define Thingstream_Modem2_getSeriousErrors(self, andClear) \
        Thingstream_Modem_getSeriousErrors((self), (andClear))

/** @deprecated renamed to Thingstream_Modem_setBearerMSS() */
#define Thingstream_Modem2_setBearerMSS(self, mss) \
        Thingstream_Modem_setBearerMSS((self), (mss))

/** @deprecated renamed to #ThingstreamModemUdpInit */
typedef ThingstreamModemUdpInit ThingstreamModem2UdpInit;

/** @deprecated renamed to #MODEM_USSD_BUFFER_LEN */
#define MODEM2_USSD_BUFFER_LEN MODEM_USSD_BUFFER_LEN

/** @deprecated renamed to #MODEM_UDP_BUFFER_LEN */
#define MODEM2_UDP_BUFFER_LEN MODEM_UDP_BUFFER_LEN

#if defined(__cplusplus)
}
#endif

#endif /* INC_MODEM2_TRANSPORT_H */
