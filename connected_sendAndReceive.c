/*
 * Copyright 2023-2024 Thingstream AG
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
 * @brief Example to demonstrate publish and receive when device
 * is always in a Connected state
 */

#include <string.h>

#include "run_example.h"
#include "platform_delay.h"


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

/* ---------------- Configuration section ---------------- */

static const char publish_topic_name[] = "publish_test";
static const char subscribe_topic_name[] = "test_receive";

static uint32_t publish_interval = 60*60; /* time in seconds */
static uint32_t msg_check_interval = 5;   /* time in seconds */

/* ------------------------------------------------------ */

/* Saved 'modem transport' setup when Thingstream stack is created.
 * This is required if the application needs to speak directly to
 * the modem using Thingstream_Modem_sendLine().
 */
static ThingstreamTransport *modem_transport;

/* Saved 'client' value setup in publish_and_get_messages() */
static ThingstreamClient *client;

/* Example publishes to this topic. Setup in publish_and_get_messages() */
static ThingstreamTopic publish_topic;

/* Example subscribes to this topic. Setup in publish_and_get_messages() */
static ThingstreamTopic receive_topic;

/* A flag to indicate that a disconnect or puback error has been notified by
 * a callback from the Thingstream SDK stack. We treat these callbacks as a
 * need to re-initialise the Thingstream SDK stack.
 * It stores 0, or a time at which the re-initialisation should occur.
 */
static uint32_t disconnectOrPubackError;

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
 * Callback for receiving messages.
 * This will be called from within Thingstream_Client_run()
 */
void Thingstream_Application_subscribeCallback(ThingstreamTopic topic,
     ThingstreamQualityOfService_t qos, uint8_t* msg, uint16_t msgLen)
{
    UNUSED(topic);
    UNUSED(qos);

    if (msgLen > 0)
    {
        Thingstream_Util_printf("Received message: %s\n", msg);
    }
}

/**
 * Connect to Thingstream if required. Publish every N seconds
 * and download any available messages.
 *
 * Set the publish interval and topic names at the top of this
 * file in the 'Configuration section'.
 *
 * This routine is called periodically from run_thingstream_example().
 */
static void publish_and_get_messages(ThingstreamTransport *transport)
{
    ThingstreamClientResult result = TRANSPORT_ERROR;

    if (client == NULL)
    {
        /* ---- Create client and connect to Thingstream --- */

        client = Thingstream_createClient(transport);
        CHECK("client", client != NULL);

        result = Thingstream_Client_init(client);
        CHECK_CLIENT_SUCCESS("client init", result, destroy);

        result = Thingstream_Client_connect(client, true, 0, NULL);
        CHECK_CLIENT_SUCCESS("connect", result, shutdown);

        /* --------------- client connected --------------- */

        /* Register to a topic for publishing.
         * Not required when using predefined or short topics.
         */
        result = Thingstream_Client_register(client, publish_topic_name,
                                                     &publish_topic);
        CHECK_CLIENT_SUCCESS("register", result, disconnect);


        /* Subscribe to the 'test' topic to access any corresponding
         * data from the Thingstream portal.
         */
        result = Thingstream_Client_subscribeName(client, subscribe_topic_name,
                                          ThingstreamQOS1, &receive_topic);
        CHECK_CLIENT_SUCCESS("subscribe", result, disconnect);

    }
    else if (disconnectOrPubackError != 0)
    {
        /**
         * The SDK has notified a disconnect or puback error.
         * After a delay (to avoid contention/confusion) we shutdown the
         * Thingstream SDK stack and clear the client variable so that the
         * SDK stack will be re-initialised.
         */
        uint32_t now = Thingstream_Platform_getTimeMillis();
        if (TIME_COMPARE(now, <, disconnectOrPubackError))
        {
            (void) Thingstream_Client_run(client, 10);
            return;
        }
        goto disconnect;
    }

    /*
     * At the appropriate interval, publish a message to Thingstream.
     *
     * Note: the interval calculated below will wrap approximately
     * every 49 days and trigger an early publish, before returning
     * to the correct publish interval.
     */

    uint32_t now = Thingstream_Platform_getTimeMillis();
    static uint32_t last_publish;
    uint32_t diff = now - last_publish;

    if (diff > publish_interval*1000)
    {
        static const char msg[] = "Hello, connect, send and receive";

        last_publish = now;
        result = Thingstream_Client_publish(client, publish_topic,
                                    ThingstreamQOS1, false,
                                    (uint8_t*)msg, sizeof(msg)-1);
        CHECK_CLIENT_SUCCESS("publish", result, disconnect);
    }

    /* As the client is in a Connected state, the server will send
     * any messages without further prompting.
     * Process any new messages with Client_run().
     */
    Thingstream_Client_run(client, 0);
    return;


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
    disconnectOrPubackError = 0;
    client = NULL;

error:
    return;
}

/**
 * Create the Thingstream Client stack and periodically
 * call publish_and_get_messages().
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
        publish_and_get_messages(transport);
        Platform_delayMillis(msg_check_interval*1000);
    }

error:
    return TRANSPORT_ERROR;
}

/* This callback is only needed if publishing at ThingstreamQOS0
 * (The example uses ThingstreamQOS1 by default.)
 */
void Thingstream_Application_disconnectCallback(void)
{
    uint32_t now = Thingstream_Platform_getTimeMillis();
    /* Cleanup as soon as possible, the " 1 | ..." handles the rare case when
     * the time is zero.
     */
    disconnectOrPubackError = 1 | now;
}

/* This callback is only needed if publishing at ThingstreamQOS0, or
 * ThingstreamQOSM1.
 * (The example uses ThingstreamQOS1 by default.)
 */
void Thingstream_Application_pubackErrorCallback(ThingstreamClientResult resultCode)
{
    UNUSED(resultCode);
    uint32_t now = Thingstream_Platform_getTimeMillis();
    /* Cleanup after a delay to allow confusion/contention to subside,
     * the " 1 | ..." handles the rare case when the time is zero.
     */
    disconnectOrPubackError = 1 | (now + (60u * 1000));
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

