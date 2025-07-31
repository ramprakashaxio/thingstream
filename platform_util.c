/*
 * Copyright 2021-2023 Thingstream AG
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
 * @brief A file to provide Thingstream debug output
 *
 */

#include "client_platform.h"
#include "sdk_config.h"

#if defined(SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS) && (SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS > 0)
#include "SEGGER_RTT.h"
#else /* SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS */
#include <stdio.h>
#endif /* SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS */

void Thingstream_Util_initOutput(void)
{
#if defined(SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS) && (SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS > 0)
    /* Setup SEGGER_RTT_Write() so that it blocks while the RTT buffer is full.
     * This blocking behaviour is needed for the Thingstream logging.
     */
    SEGGER_RTT_Init();
    SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
#endif /* SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS */
}

/**
 * Output a string to a debugging stream.
 *
 * **Optional** only needed if Thingstream_Util_printf() is called.
 * @param str the string to be written (may not be 0-terminated)
 * @param len the number of characters to write.
 */
void Thingstream_Platform_puts(const char* str, int len)
{
#if defined(SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS) && (SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS > 0)
    SEGGER_RTT_Write(0, (const void*)str, len);
#else /* SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS */
    while (len--)
    {
        char ch = *str++;
        putchar(ch);
    }
#endif /* SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS */
}
