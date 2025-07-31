/*
 * Copyright 2023 Thingstream AG
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

#ifndef INC_PLATFORM_DELAY_H_
#define INC_PLATFORM_DELAY_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * Delay for the given number of milliseconds.
 * The ideal implementation would wait for interrupts or put the processor into
 * a low power sleep mode.
 *
 * @param millis the time to delay
 */
void Platform_delayMillis(uint32_t millis);

#if defined(__cplusplus)
}
#endif


#endif /* INC_PLATFORM_DELAY_H_ */
