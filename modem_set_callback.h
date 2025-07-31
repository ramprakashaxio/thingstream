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
 * @brief **Deprecated** Thingstream_Modem*_setXxxxCallback APIs
 */

#ifndef INC_MODEM_SET_CALLBACK_H_
#define INC_MODEM_SET_CALLBACK_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include <stdint.h>
#include <stdbool.h>
#include "transport_api.h"

/**
 * @addtogroup legacy-modemCallback
 * @{
 */

/**
 * Type definition of the "modem response arrived" callback
 * @param cookie the cookie passed to Thingstream_Modem_setCallback()
 * @param response the unrecognized modem response
 * @param len the length of the response
 *
 * @deprecated Alternative Thingstream_Application_modemCallback() is preferred
 */
typedef void (*ThingstreamModemCallback_t)(void *cookie, const char* response, uint16_t len);

/**
 * Set the function that will be called when the modem receives an
 * unrecognized response.
 * @param self this instance of modem transport
 * @param callback the #ThingstreamModemCallback_t function
 * @param cookie a caller supplied opaque item passed when callback is called.
 *
 * @deprecated Alternative Thingstream_Application_modemCallback() is preferred
 */
void Thingstream_Modem_setCallback(ThingstreamTransport* self, ThingstreamModemCallback_t callback, void* cookie);

/**
 * Clear the function that will be called when the transport layer receive an
 * unrecognized response.
 * Note that calling this will indicate to the stack that the client no longer
 * wishes to accept unrecognized responses from the modem.
 * @param self this instance of modem transport
 *
 * @deprecated Alternative Thingstream_Application_modemCallback() is preferred
 */
#define Thingstream_Modem_clearCallback(self) \
    Thingstream_Modem_setCallback((self), NULL, NULL)

/** @deprecated Renamed to Thingstream_Modem_setCallback().
 * Alternative Thingstream_Application_modemCallback() is preferred
 */
extern void Thingstream_Modem2_setCallback(ThingstreamTransport* self, ThingstreamModemCallback_t callback, void* cookie);

/** @deprecated Renamed to Thingstream_Modem_clearCallback().
 * Alternative Thingstream_Application_modemCallback() is preferred
 */
#define Thingstream_Modem2_clearCallback(self) \
    Thingstream_Modem_clearCallback(self)

/** @} */

#if defined(__cplusplus)
}
#endif

#endif /* INC_MODEM_SET_CALLBACK_H_ */
