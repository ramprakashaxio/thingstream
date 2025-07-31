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
 * @brief Modem reset string
 *
 * This file defines the modem string used by the Thingstream SDK
 * to reset the modem after a serious problem.
 */

#include <modem_transport.h>

/**
 * This string is used to reset the modem after a serious problem has been
 * detected. Each entry in the string is terminated by "\n".
 * If an entry starts with a "?" then the "?" is not passed to the modem, but
 * any ERROR (or +CME ERROR:) produced will be ignored.
 *
 * If the forced reset is successful it will be followed by the normal commands
 * used to initialise the modem.
 */
const char Thingstream_Modem_forceResetString[] =
    "AT+CFUN=1,1\n"    /* Set Phone Functionality (reset, then full function) */
    "~5000\n"          /* Allow the modem to restart before we continue */
     ;
