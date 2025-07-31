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

#ifndef INC_PLATFORM_SENSOR_H_
#define INC_PLATFORM_SENSOR_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif


/**
 * Return a temperature.
 *
 * @param pTemp a pointer to the int16_t that will receive the temperature
 * @return true if a value has been returned.
 */
extern bool Platform_getTemperature(int16_t *pTemp);

/**
 * Return a pressure.
 *
 * @param pPressure a pointer to the uint16_t that will receive the pressure
 * @return true if a value has been returned.
 */
extern bool Platform_getPressure(uint16_t *pPressure);


/**
 * Return battery voltage + charge.
 *
 * @param pVoltage a pointer to the uint16_t that will receive the voltage
 * @param pCharge a pointer to the uint16_t that will receive the charge
 * @return true if a value has been returned.
 */
bool Platform_getBatteryState(uint16_t *pVoltage, uint16_t *pCharge);


#if defined(__cplusplus)
}
#endif


#endif /* INC_PLATFORM_SENSOR_H_ */
