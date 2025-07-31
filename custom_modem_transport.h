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

#ifndef INC_CUSTOM_MODEM_TRANSPORT_H
#define INC_CUSTOM_MODEM_TRANSPORT_H

#include <transport_api.h>

#include <modem_transport.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Create an instance of the modem transport.
 *
 * Parameters are port specific.
 * @return the instance
 */
extern ThingstreamTransport* Thingstream_createCustomModemTransport(/* porting specific options */);


#if defined(__cplusplus)
}
#endif

#endif /* INC_CUSTOM_MODEM_TRANSPORT_H */
