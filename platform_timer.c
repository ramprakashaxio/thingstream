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
 * @brief A provider of a count of elapsed milliseconds
 *
 * Thanks to Torbjørn (Nordic devzone user "ovrebekk") for his example which
 * was the starting point for this implementation.
 */

#include "platform_timer.h"
#include "nrf52.h"
#include "nrf52_bitfields.h"


/* Use nRF52480 TIMER1 */
#define TS_TIMER            NRF_TIMER1
#define TS_TIMER_IRQn       TIMER1_IRQn
#define TS_TIMER_IRQHandler TIMER1_IRQHandler

/* Our count of elapsed milliseconds which is incremented every timer interrupt */
static volatile uint32_t elapsedMs;

void Thingstream_Platform_initTimer(void)
{
    elapsedMs = 0;

    // Ensure the timer uses 24-bit bitmode or higher
    TS_TIMER->BITMODE = TIMER_BITMODE_BITMODE_24Bit << TIMER_BITMODE_BITMODE_Pos;

    // Set the prescaler to 4, for a timer interval of 1 us (16M / 2^4)
    TS_TIMER->PRESCALER = 4;

    // Set the CC[0] register to hit after 1 millisecond
    TS_TIMER->CC[0] = 1000;

    // Make sure the timer clears after reaching CC[0]
    TS_TIMER->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;

    // Trigger the interrupt when reaching CC[0]
    TS_TIMER->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

    // Set a low IRQ priority and enable interrupts for the timer module
    NVIC_SetPriority(TS_TIMER_IRQn, 7);
    NVIC_EnableIRQ(TS_TIMER_IRQn);

    // Clear and start the timer
    TS_TIMER->TASKS_CLEAR = 1;
    TS_TIMER->TASKS_START = 1;
}

uint32_t Thingstream_Platform_getTimeMillis(void)
{
    return elapsedMs;
}



// Timer interrupt handler (called once per second)
void TS_TIMER_IRQHandler(void)
{
    if (TS_TIMER->EVENTS_COMPARE[0])
    {
        TS_TIMER->EVENTS_COMPARE[0] = 0;
        elapsedMs++;
    }
}
