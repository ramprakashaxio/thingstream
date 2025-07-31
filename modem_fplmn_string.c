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
 * @brief Modem clear FPLMN string
 *
 * This file defines the modem string used by the Thingstream SDK
 * to clear the SIM's Forbidden Operator List.
 */

#include <modem_transport.h>

/**
 * This string is used to read the SIM's FPLMN list of barred operators.
 * The SDK modem driver issues this string when it has been unable to
 * register with an operator.
 */
const char Thingstream_Modem_readFplmnString[] =
    "?AT+CRSM=176,28539,0,0,12\n" /* Read FPLMN */
     ;

/**
 * This string is used to clear the SIM's FPLMN list of barred operators.
 * The SDK modem driver issues this string (and the sequence from the
 * #Thingstream_Modem_forceResetString) when it has been unable to register with
 * an operator.
 */
const char Thingstream_Modem_clearFplmnString[] =
    "?AT+CRSM=214,28539,0,0,12,\"FFFFFFFFFFFFFFFFFFFFFFFF\"\n" /* Clear FPLMN */
     ;
