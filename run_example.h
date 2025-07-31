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
 * @brief Macros and prototypes used by the shared examples
 */

#ifndef INC_RUN_EXAMPLE_H_
#define INC_RUN_EXAMPLE_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include "flags.h"  /* configuration */

#include "thingstream.h"

/* Macro to eliminate warnings about unused parameters. */
#undef UNUSED
#define UNUSED(symbol) ((void)symbol)

/* Macros to check and report on the example's progress */
#define CHECK(msg, truth)                                      \
    do                                                         \
    {                                                          \
        if (truth)                                             \
        {                                                      \
            Thingstream_Util_printf("%s ok\n", msg);           \
        }                                                      \
        else                                                   \
        {                                                      \
            Thingstream_Util_printf("%s ERROR at line %d\n", msg, __LINE__); \
            goto error;                                         \
        }                                                      \
    } while (0)

#define CHECK_CLIENT_SUCCESS(msg, cr, label)                   \
    do                                                         \
    {                                                          \
        if (cr == CLIENT_SUCCESS)                              \
        {                                                      \
            Thingstream_Util_printf("%s ok\n", msg);           \
        }                                                      \
        else                                                   \
        {                                                      \
            Thingstream_Util_printf("%s ERROR result=%d[%s] at line %d\n", msg, cr, Thingstream_Client_getErrorText(cr), __LINE__); \
            goto label;                                        \
        }                                                      \
    } while (0)

/**
 * Run the example.
 * @param transport the serial instance to allow the example to
 *      interact with the modem
 * @param modem_init indicates either USSD, or which UDP modem to use
 * @param modem_flags to pass to Thingstream_createModemTransport()
 * @return 0 for success, non-zero for failure
 */
extern ThingstreamClientResult run_example(ThingstreamTransport *transport,
                               ThingstreamModemUdpInit *modem_init,
                               uint16_t modem_flags);

#if defined(__cplusplus)
}
#endif

#endif /* INC_RUN_EXAMPLE_H_ */

