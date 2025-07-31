/*
 * Copyright 2021-2024 Thingstream AG
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
 * @brief Example to demonstrate publishing sensor data to predefined
 * topics.
 */

#include <stdio.h>
#include <string.h>

#include "run_example.h"
#include "platform_delay.h"
#include "platform_sensor.h"

typedef struct Sensor_s
{
    int16_t temperature;
    uint16_t pressure;
    uint16_t voltage;
    uint16_t charge;
} Sensor;


/* --------- Setup buffer for modem transport ---------- */
/* Define a buffer for use with the
 * Thingstream_createModemTransport() routine.
 *
 * UDP  suggestion: MODEM_UDP_BUFFER_LEN
 * USSD suggestion: MODEM_USSD_BUFFER_LEN
 */
#ifndef MODEM_BUFFER_LEN
#define MODEM_BUFFER_LEN MODEM_UDP_BUFFER_LEN
#endif
static uint8_t modemBuf[MODEM_BUFFER_LEN];
/* ------------------------------------------------------ */

/* ------------ Setup buffer for uart data  ------------ */
/* Some targets need a buffer to store data read from the
 * modem before it can be processed by the SDK, e.g. if
 * the data is read directly from an ISR.
 */
#ifndef RING_BUFFER_LENGTH
#define RING_BUFFER_LENGTH 250
#endif
/* ------------------------------------------------------ */

/* Saved 'modem transport' setup when Thingstream stack is created.
 * This is required if the application needs to speak directly to
 * the modem using Thingstream_Modem_sendLine().
 */
static ThingstreamTransport *modem_transport;

const ThingstreamTopic PredefinedTemperatureTopic    = MAKE_PREDEFINED_TOPIC(300);
const ThingstreamTopic PredefinedPressureTopic       = MAKE_PREDEFINED_TOPIC(301);
const ThingstreamTopic PredefinedBatteryVoltageTopic = MAKE_PREDEFINED_TOPIC(302);
const ThingstreamTopic PredefinedBatteryChargeTopic  = MAKE_PREDEFINED_TOPIC(303);

static Sensor previous;

/*
 * Limit the total number of publishes, to avoid consuming too
 * many messages during the test run.
 */
#define PUBLISH_COUNT_LIMIT 10
static int count;

/*
 * A helper to report an error when the Thingstream server has rejected the
 * publish because the 'predefined' topic ID we are using in this application
 * does not match a valid predefined topic alias on the server.
 *
 * @param topic the predefined topic that the application is using
 * @param topicName the suggested name of the topic on the server
 */
static void reportInvalidTopic(const ThingstreamTopic topic, const char* topicName)
{
    if (topic.topicType == topicTypePredefined)
    {
        Thingstream_Util_printf("\n\n"
                                "===================================="
                                "====================================\n"
                                "The predefined topic '%s' with alias %d\n"
                                "has not been defined in the Thingstream portal.\n"
                                "===================================="
                                "====================================\n\n\n",
                                topicName, topic.topicId);
    }
}


/**
 * Default (dummy) implementation to return a temperature.
 * This is used for testing before implementation using real hardware
 * is available. If the application provides an alternative implementation
 * then it will be used instead of this dummy version.
 * To disable this dummy value this routine should return false.
 *
 * @param pTemp a pointer to the int16_t that will receive the temperature
 * @return true if a value has been returned.
 */
__attribute__((weak))
bool Platform_getTemperature(int16_t *pTemp)
{
    /* Pretend that the temperature readings will not change for 120 seconds */
    uint32_t now = Thingstream_Platform_getTimeMillis();
    static uint32_t last;
    uint32_t diff = now - last;
    if (diff > 120000)
    {
        last = now;
        *pTemp = previous.temperature + (now >> 14);
        return true;
    }
    return false;
}


/**
 * Default (dummy) implementation to return a pressure.
 * This is used for testing before implementation using real hardware
 * is available. If the application provides an alternative implementation
 * then it will be used instead of this dummy version.
 * To disable this dummy value this routine should return false.
 *
 * @param pPressure a pointer to the uint16_t that will receive the pressure
 * @return true if a value has been returned.
 */
__attribute__((weak))
bool Platform_getPressure(uint16_t *pPressure)
{
    /* Pretend that the pressure readings will not change for 180 seconds */
    uint32_t now = Thingstream_Platform_getTimeMillis();
    static uint32_t last;
    uint32_t diff = now - last;
    if (diff > 180000)
    {
        last = now;
        *pPressure = previous.pressure + (now >> 15);
        return true;
    }
    return false;
}


/**
 * Default (dummy) implementation to return battery voltage + charge.
 * This is used for testing before implementation using real hardware
 * is available. If the application provides an alternative implementation
 * then it will be used instead of this dummy version.
 * To disable this dummy value this routine should return false.
 *
 * @param pVoltage a pointer to the uint16_t that will receive the voltage
 * @param pCharge a pointer to the uint16_t that will receive the charge
 * @return true if a value has been returned.
 */
__attribute__((weak))
bool Platform_getBatteryState(uint16_t *pVoltage, uint16_t *pCharge)
{
    /* Pretend that the battery readings will not change for 300 seconds */
    uint32_t now = Thingstream_Platform_getTimeMillis();
    static uint32_t last;
    uint32_t diff = now - last;
    if (diff > 300000)
    {
        last = now;
        uint16_t adjustVoltage = (now % 50);
        if (previous.voltage > adjustVoltage)
            *pVoltage = (previous.voltage - adjustVoltage);
        else
            *pVoltage = (previous.voltage + (adjustVoltage * 100));
        uint16_t adjustCharge = ((now >> 8) % 50);
        if (previous.charge > adjustCharge)
            *pCharge = (previous.charge - adjustCharge);
        else
            *pCharge = (previous.charge + (adjustCharge * 100));
        return true;
    }
    return false;
}

/**
 * Default (dummy) implementation to delay for the given number of milliseconds.
 * A better implementation would wait for interrupts or put the processor into
 * a low power sleep mode.
 *
 * @param millis the time to delay
 */
__attribute__((weak))
void Platform_delayMillis(uint32_t millis)
{
    uint32_t now = Thingstream_Platform_getTimeMillis();
    uint32_t when = now + millis;
    while ((int32_t)(when - now) > 0)
    {
        now = Thingstream_Platform_getTimeMillis();
    }
}


/**
 * Read the sensors and if they have changed then publish the new values
 * to the Thingstream server.
 *
 * @param transport the Thingstream SDK transport to use
 */
static void publish_sensor_changes(ThingstreamTransport *transport)
{
    ThingstreamClientResult result = TRANSPORT_ERROR;

    /* Obtain current values of all the sensors, if any sensor gives an
     * error then just set the current value to the previous value so that
     * we will skip the publish below.
     */
    Sensor current;
    if (!Platform_getTemperature(&current.temperature))
    {
        current.temperature = previous.temperature;
    }
    if (!Platform_getPressure(&current.pressure))
    {
        current.pressure = previous.pressure;
    }
    if (!Platform_getBatteryState(&current.voltage, &current.charge))
    {
        current.voltage = previous.voltage;
        current.charge  = previous.charge;
    }

    /* If no sensor reading has changed, just return */

    if ((current.temperature == previous.temperature)
     && (current.pressure == previous.pressure)
     && (current.voltage == previous.voltage)
     && (current.charge == previous.charge))
    {
        return;
    }

    /* Publish any updated values, so create client and connect */

    ThingstreamClient* client = Thingstream_createClient(transport);
    CHECK("client", client != NULL);

    result = Thingstream_Client_init(client);
    CHECK_CLIENT_SUCCESS("client init", result, destroy);

    result = Thingstream_Client_connect(client, true, 0, NULL);
    CHECK_CLIENT_SUCCESS("connect", result, shutdown);

    /* If the temperature has changed then publish an update */
    char msg[32];
    if (current.temperature != previous.temperature)
    {
        sprintf(msg, "%d", current.temperature);
        result = Thingstream_Client_publish(client, PredefinedTemperatureTopic,
                                            ThingstreamQOS1, false,
                                            (uint8_t*) msg, strlen(msg));
        if (result == CLIENT_TOPIC_INVALID)
        {
            reportInvalidTopic(PredefinedTemperatureTopic,
                               "sensor/temperature");
        }
        else
        {
            CHECK_CLIENT_SUCCESS("publish temperature", result, disconnect);
        }
        previous.temperature = current.temperature;
        count++;
    }

    /* If the pressure has changed then publish an update */
    if (current.pressure != previous.pressure)
    {
        sprintf(msg, "%d", current.pressure);
        result = Thingstream_Client_publish(client, PredefinedPressureTopic,
                                            ThingstreamQOS1, false,
                                            (uint8_t*) msg, strlen(msg));
        if (result == CLIENT_TOPIC_INVALID)
        {
            reportInvalidTopic(PredefinedPressureTopic,
                               "sensor/pressure");
        }
        else
        {
            CHECK_CLIENT_SUCCESS("publish pressure", result, disconnect);
        }
        previous.pressure = current.pressure;
        count++;
    }

    /* If the battery readings have changed then publish an update */
    if (current.voltage != previous.voltage)
    {
        sprintf(msg, "%d", current.voltage);
        result = Thingstream_Client_publish(client, PredefinedBatteryVoltageTopic,
                                            ThingstreamQOS1, false,
                                            (uint8_t*) msg, strlen(msg));
        if (result == CLIENT_TOPIC_INVALID)
        {
            reportInvalidTopic(PredefinedBatteryVoltageTopic,
                               "sensor/battery/voltage");
        }
        else
        {
            CHECK_CLIENT_SUCCESS("publish voltage", result, disconnect);
        }
        previous.voltage = current.voltage;
        count++;
    }
    if (current.charge != previous.charge)
    {
        sprintf(msg, "%d", current.charge);
        result = Thingstream_Client_publish(client, PredefinedBatteryChargeTopic,
                                            ThingstreamQOS1, false,
                                            (uint8_t*) msg, strlen(msg));
        if (result == CLIENT_TOPIC_INVALID)
        {
            reportInvalidTopic(PredefinedBatteryChargeTopic,
                               "sensor/battery/charge");
        }
        else
        {
            CHECK_CLIENT_SUCCESS("publish charge", result, disconnect);
        }
        previous.charge = current.charge;
        count++;
    }

    ThingstreamClientResult cr;
disconnect:
    cr = Thingstream_Client_disconnect(client, 0);
    CHECK_CLIENT_SUCCESS("disconnect", cr, shutdown);

shutdown:
    cr = Thingstream_Client_shutdown(client);
    CHECK_CLIENT_SUCCESS("shutdown", cr, destroy);

destroy:
    cr = Thingstream_Client_destroy(client);
    CHECK_CLIENT_SUCCESS("destroy", cr, error);

error:
    return;
}

/**
 * Periodically read sensors and if the readings have changed then
 * connect to the Thingstream server and publish any changes at QoS1
 * using a predefined topic.
 */
ThingstreamClientResult run_example(ThingstreamTransport *transport,
                        ThingstreamModemUdpInit *modem_init,
                        uint16_t modem_flags)
{

    /* --------- Create the Thingstream SDK stack ---------- */

    static uint8_t ringBuffer[RING_BUFFER_LENGTH];

    transport = Thingstream_createRingBufferTransport(transport, ringBuffer,
                                                        sizeof(ringBuffer));
    CHECK("ring_buffer", transport != NULL);

#if (defined(DEBUG_LOG_MODEM) && (DEBUG_LOG_MODEM > 0))
    transport = Thingstream_createModemLogger(transport,
                                              Thingstream_Util_printf,
                                              TLOG_TRACE | TLOG_TIME);
    CHECK("log_modem", transport != NULL);
#endif /* DEBUG_LOG_MODEM */

    transport = Thingstream_createModemTransport(transport,
                                                 modem_flags,
                                                 modemBuf, sizeof(modemBuf),
                                                 modem_init);
    CHECK("modem", transport != NULL);

    /* Save a copy of the modem layer transport in case it is
     * needed for passing to Thingstream_Modem_sendLine().
     */
    modem_transport = transport;

    /* Base 64 encoding is optional when using UDP. */
    transport = Thingstream_createBase64CodecTransport(transport);
    CHECK("base64", transport != NULL);

    transport = Thingstream_createProtocolTransport(transport, NULL, 0);
    CHECK("thingstream", transport != NULL);

#if (defined(DEBUG_LOG_CLIENT) && (DEBUG_LOG_CLIENT > 0))
    transport = Thingstream_createClientLogger(transport,
                                               Thingstream_Util_printf,
                                               TLOG_TRACE | TLOG_TIME);
    CHECK("log_client", transport != NULL);
#endif /* DEBUG_LOG_CLIENT */


    for (;;)
    {
        if (count >= PUBLISH_COUNT_LIMIT)
        {
            Thingstream_Util_printf("sensor_predefined_topic completed\n");
            return TRANSPORT_SUCCESS;
        }
        publish_sensor_changes(transport);
        Platform_delayMillis(30000);
    }

error:
    return TRANSPORT_ERROR;
}

/*
 * As documented, it saves a little space to provide empty callback
 * functions, to replace the default versions which implement backwards
 * compatability.
 */
void Thingstream_Application_modemCallback (const char *response, uint16_t len)
{
    UNUSED(response); UNUSED(len);
}
void Thingstream_Application_registerCallback (const char *topicName, ThingstreamTopic topic)
{
    UNUSED(topicName); UNUSED(topic);
}
void Thingstream_Application_subscribeCallback (ThingstreamTopic topic, ThingstreamQualityOfService_t qos, uint8_t *payload, uint16_t payloadlen)
{
    UNUSED(topic); UNUSED(qos); UNUSED(payload); UNUSED(payloadlen);
}
