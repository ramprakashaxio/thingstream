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
 * @brief Example to demonstrate publish and receive: reducing
 * message count and modem up-time
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


static bool msg_received = false;   /* updated in subscribeCallback */

/**
 * Callback for receiving messages.
 * This will be called from within Thingstream_Client_run()
 */
void Thingstream_Application_subscribeCallback(ThingstreamTopic topic,
     ThingstreamQualityOfService_t qos, uint8_t* msg, uint16_t msgLen)
{
    UNUSED(topic);
    UNUSED(qos);

    if ((msgLen > 0))
    {
        msg_received = true;
        Thingstream_Util_printf("Received message: %s\n", msg);
    }
}

/**
 * Create the Thingstream Client stack, publish at QoS -1,
 * retrieve any messages using 'ping'; re-establishing the
 * sleep state if required.
 *
 * This example reduces message usage by:
 * - publishing at QoS -1 (no connection required)
 * - publishing to a predefined topic (no registration required)
 * - using the 'get messages' API (info is returned for 'free' from the publish)
 * - using the auto-subscribed predefined self-topic to receive messages
 *
 * This example reduces modem up-time by:
 * - remaining in a 'sleep' state and retrieving messages via 'ping',
 * thus the modem can be powered down when not actively publishing or
 * fetching messages.
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

    /* Publish data then retrieve any messages waiting on the
     * server.
     */

    static const char msg[] = "Hello publish sleep";
    result = Thingstream_Client_publish(client, Thingstream_PredefinedSelfTopic,
                                    ThingstreamQOSM1, false,
                                    (uint8_t*) msg, sizeof(msg)-1);
    CHECK_CLIENT_SUCCESS("publish", result, shutdown);


    /* Check for inbound messages.
     *
     * As part of the Thingstream_Client_publish() communications the client
     * usually receives the number of queued messages from the server.
     * Retrieve this information so we can avoid a ping.
     */
    result = Thingstream_Client_messageWaiting(client);
    Thingstream_Util_printf("MessageWaiting check returned %d\n", (int)result);

    if (result > 0)
    {
        /* Ask for messages (only works if connected or asleep) */
        Thingstream_Util_printf("Ask for waiting messages...\n");
        result = Thingstream_Client_ping(client);
        Thingstream_Util_printf("ping returned %d\n", (int)result);
    }
    else if (result == CLIENT_INFORMATION_NOT_AVAILABLE)
    {
        /* Thingstream_Client_messageWaiting() can return this when the
         * the device has never previously connected or pinged the server.
         * Do a connection below and in future
         * Thingstream_Client_messageWaiting() will return valid information.
         */
    }

    if ((result == CLIENT_NOT_CONNECTED)
        || (result == CLIENT_INFORMATION_NOT_AVAILABLE))
    {
        /* The Thingstream_Client_ping() reported that we are not connected or
         * the Thingstream_Client_messageWaiting() failed to provide any
         * information. Both problems are resolved by establishing the "asleep"
         * MQTT-SN state with the server by disconnecting with a large timeout,
         * but to do that we must first connect.
         * Use a short keepAlive time so that if network errors prevent
         * the disconnect getting through, the server will disconnect
         * automatically.
         */
        Thingstream_Util_printf("Connecting...\n");

        result = Thingstream_Client_connect(client, false, 3, NULL);
        CHECK_CLIENT_SUCCESS("connect", result, shutdown);

        /* Restore the sleeping state (use a sleep time of 25 hours) */
        Thingstream_Util_printf("Disconnecting to set sleep state...\n");
        result = Thingstream_Client_disconnect(client, 25 * 60);
        CHECK_CLIENT_SUCCESS("disconnect", result, shutdown);

        /* Repeat the check to see if there are waiting inbound messages.
         * The Thingstream_Client_connect() call above will have been
         * informed of waiting messages.
         */
        result = Thingstream_Client_messageWaiting(client);
        Thingstream_Util_printf("MessageWaiting check says = %d\n", result);
        if (result > 0)
        {
            Thingstream_Util_printf("Ask for waiting messages...\n");
            result = Thingstream_Client_ping(client);
            CHECK_CLIENT_SUCCESS("ping", result, shutdown);
        }
    }

    ThingstreamClientResult cr;
shutdown:
    cr = Thingstream_Client_shutdown(client);
    CHECK_CLIENT_SUCCESS("shutdown", cr, destroy);

destroy:
    cr = Thingstream_Client_destroy(client);
    CHECK_CLIENT_SUCCESS("destroy", cr, error);

error:
    /* Thingstream activity is complete.
     * Modem can now be turned off.
     */
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

