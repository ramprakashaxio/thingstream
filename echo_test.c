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
 * @brief Example to demonstrate message send and receive
 */

#include <string.h>

#include "run_example.h"


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


/* Specify topic name. Will be used for publish and subscription. */
#define EXAMPLE_TOPIC "test/thingstream/echo"
static uint16_t exampleTopicId = 0;  /* will be set by subscription */

static bool msg_received = false;   /* updated in subscribeCallback */

/**
 * Callback for receiving messages.
 * This will be called from within Thingstream_Client_run()
 */
void Thingstream_Application_subscribeCallback(ThingstreamTopic topic,
     ThingstreamQualityOfService_t qos, uint8_t* msg, uint16_t msgLen)
{
    UNUSED(qos);
    if ((topic.topicId == exampleTopicId) && (msgLen > 0))
    {
        msg_received = true;
        Thingstream_Util_printf("Received message: %s\n", msg);
    }
}

/**
 * Create the Thingstream Client stack, subscribe to the test
 * topic, publish to the test topic at QoS 1 and expect to
 * receive the published message echoed back to the device.
 *
 * This example shows basic 'send and receive' behaviour using
 * a single topic for simplicity. However, in practice a device
 * would generally not subscribe and publish to the same topic.
 */
ThingstreamClientResult run_example(ThingstreamTransport *transport,
                        ThingstreamModemUdpInit *modem_init,
                        uint16_t modem_flags)
{
    ThingstreamClientResult result = TRANSPORT_ERROR;

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

    ThingstreamClient *client;
    client = Thingstream_createClient(transport);
    CHECK("client", client != NULL);

    result = Thingstream_Client_init(client);
    CHECK_CLIENT_SUCCESS("client init", result, destroy);

    /* ----------- Stack created ---------------------------- */

    /* Subscribe, publish, wait for the echo'ed message then
     * tidy-up by shutting down the client.
     */
    ThingstreamTopic topic;

    result = Thingstream_Client_connect(client, true, 0, NULL);
    CHECK_CLIENT_SUCCESS("connect", result, shutdown);

    /* Subscribe to the 'test topic' to receive the published
     * message back from the server.
     */
    result = Thingstream_Client_subscribeName(client, EXAMPLE_TOPIC,
                                          ThingstreamQOS1, &topic);
    CHECK_CLIENT_SUCCESS("subscribe", result, disconnect);
    exampleTopicId = topic.topicId;

    static const char msg[] = "Hello from Echo test";
    result = Thingstream_Client_publish(client, topic, ThingstreamQOS1,
                                    false, (uint8_t*) msg, sizeof(msg)-1);
    CHECK_CLIENT_SUCCESS("publish", result, disconnect);

    /* Wait for up to a minute for the message to be echo'ed
     * back.
     */
    uint32_t now = Thingstream_Platform_getTimeMillis();
    uint32_t limit = now + 60000;
    do
    {
        Thingstream_Client_run(client, 1000);
        now = Thingstream_Platform_getTimeMillis();
    }
    while (TIME_COMPARE(now, <, limit) && !msg_received);

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
    return result;
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

