/*
 * Copyright 2019-2021 Thingstream AG
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


#include <stdio.h>
#include <stdlib.h>

#include "platform_timer.h"
#include "platform_util.h"
#include "application.h"

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
int main(void)
{
    /* Start a hardware timer to provide a count of elapsed milliseconds */
    Thingstream_Platform_initTimer();

    /* Initialise the debug output port to use the Segger RTT port in blocking mode */
    Thingstream_Util_initOutput();

    runApplication();

}

/*************************** End of file ****************************/
