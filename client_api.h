/*
 * Copyright 2017-2024 Thingstream AG
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
 * @brief The Thingstream Client API
 */

#ifndef INC_CLIENT_API_H_
#define INC_CLIENT_API_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include <stdint.h>
#include <stdbool.h>
#include "transport_api.h"
#include "sdk_data.h"

/* The definition of ThingstreamResult is in thingstream_result.h */
#include "thingstream_result.h"

/**
 * The subset of #ThingstreamResult values that start with CLIENT_
 *
 * See #ThingstreamResult for details of individual values.
 */
typedef ThingstreamResult ThingstreamClientResult;


/**
 * Opaque type definition for a client instance.
 */
typedef struct ThingstreamClient_s ThingstreamClient;


/**
 * @enum TopicType
 *
 * The MQTTSN type of a topic defines the interpretation of the 16-bit topicId
 * field in the ThingstreamTopic structure.
 */
enum TopicType
{
    /** a normal topic type uses a 16-bit topicId which is obtained by
     * converting the topic name with the Thingstream_Client_register() API.
     */
    topicTypeNormal,

    /** a predefined topic type uses a 16-bit topicId which has been defined
     * via the Thingstream Management Console Topics page.
     */
    topicTypePredefined,

    /** a short topic type uses a two character name stored in the 16-bits of
     * the topicId.
     */
    topicTypeShort
};

/**
 * Structure definition for a ThingstreamTopic.
 * This holds the topic type::id pair and can be one of
 *   - registered topic #topicTypeNormal\::NNNN
 *   - predefined topic #topicTypePredefined\::PPPP
 *   - short-code topic #topicTypeShort\::AABB
 */
typedef struct ThingstreamTopic_s {
    /** The type of the topic (this is actually of type enum #TopicType) */
    uint16_t topicType;
    /** The identifier of the topic */
    uint16_t topicId;
} ThingstreamTopic;

/** Initialise predefined ThingstreamTopic with given Id
 *
 * Usage: Topic my_topic = MAKE_PREDEFINED_TOPIC(id)
 *
 * On gcc, it can also be used as a literal, with a cast.
 * Eg  Thingstream_Client_publish(client, (ThingstreamTopic)MAKE_PREDEFINED_TOPIC(id), ...)
 */
#define MAKE_PREDEFINED_TOPIC(id)  {              \
     .topicType = (uint16_t) topicTypePredefined, \
     .topicId = id                                \
}
/** Initialise short ThingstreamTopic with given character pair.
 * Usage: Topic my_topic = MAKE_SHORT_TOPIC('x', 'y')
 *
 * On gcc, it can also be used as a literal, with a cast.
 * Eg  Thingstream_Client_publish(client, (ThingstreamTopic)MAKE_SHORT_TOPIC('x', 'y'), ...)
 */
#define MAKE_SHORT_TOPIC(first, second)     {   \
     .topicType = (uint16_t) topicTypeShort,    \
     .topicId = ( (first) << 8) | (second)      \
}

/**
 * The default connection timeout used by
 * Thingstream_Client_connect() when 0 is passed as
 * the keepAlive value.
 */
#define THINGSTREAM_DEFAULT_CONNECT_KEEPALIVE 360

/**
 * The quality of service (QoS) assigned to a message.
 * There are three levels of QoS (0, 1, 2) which can be used while
 * connected for subscribe or publish. In addition,
 * QoS -1 is available and is equivalent to
 * QoS 0, but can be used to publish messages with #topicTypeShort or
 * #topicTypePredefined without connecting.
 */
typedef enum ThingstreamQualityOfService_e
{
    /** Fire and forget without the need to be connected (publish only) */
    ThingstreamQOSM1 = -1,
    /** Fire and forget - the message may not be delivered */
    ThingstreamQOS0 = 0,
    /** At least once - the message may be delivered more than once */
    ThingstreamQOS1 = 1,
    /** Once and only once - the message will be delivered exactly once */
    ThingstreamQOS2 = 2
} ThingstreamQualityOfService_t;



/**
 * This API is called by the SDK when the server sends the Client a message.
 * The application should define this routine if it wishes to receive inbound
 * messages.
 *
 * This callback will only be called from inside one of the Thingstream apis
 * (usually from Thingstream_Client_run() or Thingstream_Client_ping()).
 *
 * It is not permitted to call other Thingstream_Client_xxx() apis from within
 * this callback.
 *
 * Any data passed to the callback may be overwritten as soon as this callback
 * returns.
 *
 * @ingroup porting-application
 * @param topic the #ThingstreamTopic that this message was sent to
 * @param qos the #ThingstreamQualityOfService_t of the message
 * @param payload a pointer to the payload data (data is 0-terminated)
 * @param payloadlen the length of the payload
 */
void Thingstream_Application_subscribeCallback(ThingstreamTopic topic, ThingstreamQualityOfService_t qos, uint8_t* payload, uint16_t payloadlen);

/**
 * This API is called by the SDK when the server sends the Client a mapping
 * between the name of a topic and the topic ID. This mapping is sent just prior
 * to the first message of that topic during the current connection.
 *
 * This callback will only be called from inside one of the Thingstream apis
 * (usually from Thingstream_Client_run() or Thingstream_Client_ping()).
 *
 * It is not permitted to call other Thingstream_Client_xxx() apis from within
 * this callback.
 *
 * Any data passed to the callback may be overwritten as soon as this callback
 * returns.
 *
 * @ingroup porting-application
 * @param topicName the name of the topic
 * @param topic the #ThingstreamTopic that matches the topicName
 */
void Thingstream_Application_registerCallback(const char* topicName, ThingstreamTopic topic);

/**
 * This API is called by the SDK when the server sends a disconnect message.
 * This result is usually also made available via the return code of a
 * client call, but a publish with quality #ThingstreamQOS0, no reply
 * from the server is expected.
 *
 * This callback will only be called from inside one of the Thingstream apis.
 *
 * @ingroup porting-application
 */
void Thingstream_Application_disconnectCallback(void);

/**
 * The MQTT-SN specification for PUBLISH with quality #ThingstreamQOSM1
 * or #ThingstreamQOS0 does not include PUBACK as part of the protocol
 * <b>unless</b> there is an error status.
 * Hence the Thingstream_Client_publish() with these two qualities does not
 * return any error status to the application.
 * This API is called by the SDK when the server sends a PUBACK error.
 * It usually indicates that the client and server have become out of sync.
 *
 * This callback will only be called from inside the next Thingstream api
 * to be called after a failed Thingstream_Client_publish().
 *
 * @ingroup porting-application
 * @param resultCode an error result code matching the value returned by the
 *                   server and one of
 *
 * - #CLIENT_TOPIC_INVALID             the topic ID was invalid
 * - #CLIENT_CONGESTION                unable to publish (congestion)
 * - #CLIENT_FEATURE_NOT_IMPLEMENTED   unable to publish (unsupported)
 */
void Thingstream_Application_pubackErrorCallback(ThingstreamClientResult resultCode);

/**
 * Create a new client
 *
 * @param transport the #ThingstreamTransport instance
 * @return a pointer to a #ThingstreamClient instance, or NULL if the client instance
 *         cannot be created
 */
extern ThingstreamClient* Thingstream_createClient(ThingstreamTransport * transport);

/**
 * Destroy the client
 *
 * Currently a no-op implemented as a macro.
 *
 * @param client the #ThingstreamClient instance
 * @return a #ThingstreamClientResult indicating success/fail
 */
#define Thingstream_Client_destroy(client) ((void)(client), CLIENT_SUCCESS)

/**
 * Initialise anything that needs initialising, etc
 *
 * @param client the #ThingstreamClient instance
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_init(ThingstreamClient* client);

/**
 * Shut down the client, free any resources etc.
 * After this call the application is free to turn off the modem.
 * Before using any more Thingstream_Client_xxx() APIs, the application must call
 * Thingstream_Client_init() again to re-initialise the #ThingstreamTransport stack.
 *
 * @param client the #ThingstreamClient instance
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_shutdown(ThingstreamClient* client);

/**
 * Connect to the server.
 *
 * @param client the #ThingstreamClient instance
 * @param cleanSession set non-zero if the client wants a clean MQTT session
 * @param keepAlive the duration (in minutes) that the server will keep the
 *        connection active after receiving a message from the client.
 *        A value of zero selects #THINGSTREAM_DEFAULT_CONNECT_KEEPALIVE.
 * @param domainKey a unique identifier for the client (use NULL to obtain
 *        a value from the transport layer).
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_connect(ThingstreamClient* client, bool cleanSession, uint16_t keepAlive, const char *domainKey);

/**
 * Either enter a sleeping state, or disconnect from the server.
 * The server will persist all current subscriptions, which may then be
 * cleared out by the server on the next "cleanSession" connect.
 * @param client the #ThingstreamClient instance
 * @param duration if non-zero the number of minutes that the server should
 *        treat the client as sleeping. During this time, Thingstream_Client_ping()
 *        can be used to retrieve messages and refresh the timeout.
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_disconnect(ThingstreamClient* client, uint16_t duration);


/**
 * Test if the server has indicated that there are queued messages waiting on
 * the server for this device.
 *
 * The server sends extra data when executing the APIs
 * Thingstream_Client_publish(), Thingstream_Client_connect(), or
 * Thingstream_Client_ping() and this is remembered by the Client SDK.
 * The extra data is sent by the server at the start of the publish and at the
 * end of the connect or the ping.
 *
 * This stored number of queued messages can be queried using
 * Thingstream_Client_messageWaiting()
 *
 * Note that this is currently implemented as a macro accessing a global variable
 * but the implementation may change in the future.
 *
 * @param client the #ThingstreamClient instance (not currently used, but
 *        may be required in future).
 * @return
 *    <b>>= 0</b>: the number of queued messages on the server at the time of
 *          the previous publish, ping or connect.
 * @return
 *    #CLIENT_INFORMATION_NOT_AVAILABLE: the Client SDK failed to receive the
 *          expected extra data with the last publish, connect or ping.
 *          E.g. If the device has never previously connected to (or pinged) the
 *          server then the server is unable to send this information with the
 *          publish.
 */
#define Thingstream_Client_messageWaiting(client)                \
    ((ThingstreamClientResult)SDK_DATA_INBOUND_QUEUE(size))


/**
 * Perform an MQTT-SN ping refreshing the keepAlive or sleep timer and
 * flushing pending subscribed messages (each message will be delivered via
 * the Thingstream_Application_subscribeCallback()).
 *
 * The Ping Flush setting on the Thingstream Management Console may cause the
 * server to complete the ping request before all pending messages have been
 * sent to the device. If this is important to the application then the
 * Thingstream_Client_messageWaiting() API can be used to identify this early
 * return case.
 *
 * @param client the #ThingstreamClient instance
 * @param domainKey a unique identifier for the client (use NULL to obtain
 *        a value from the transport layer).
 * @return a #ThingstreamClientResult indicating success/fail
 * @return
 *    #CLIENT_SUCCESS:       the command completed normally
 * @return
 *    #CLIENT_NOT_CONNECTED: unable to retrieve messages, the application must
 *                          reconnect to the server first.
 *
 *    other negative results indicate some other error condition.
 */
extern ThingstreamClientResult Thingstream_Client_pingReq(ThingstreamClient* client, const char *domainKey);

/** @see Thingstream_Client_pingReq() */
#define Thingstream_Client_ping(client)  Thingstream_Client_pingReq(client, NULL)

/**
 * Register the named topic by asking the gateway for the topicId
 *
 * @param client the #ThingstreamClient instance
 * @param topicName the name of the topic to be registered
 * @param pOutTopic a pointer to the #ThingstreamTopic to receive the topic type and id.
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_register(ThingstreamClient* client, const char* topicName, ThingstreamTopic *pOutTopic);


/**
 * Send a message to the given topic.
 * @param client the #ThingstreamClient instance
 * @param topic the #ThingstreamTopic (type and id) to publish onto
 * @param qos the #ThingstreamQualityOfService_t required
 * @param retained if true, server will keep message for future subscribers
 * @param payload a pointer to the payload
 * @param payloadlen the length of the payload
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_publish(ThingstreamClient* client, ThingstreamTopic topic, ThingstreamQualityOfService_t qos, bool retained, uint8_t* payload, uint16_t payloadlen);

/**
 * Subscribe to receive messages that are published on the named topic.
 * When messages arrive the Thingstream_Application_subscribeCallback() will
 * be called with the topic, QoS, and message payload.
 *
 * @param client the #ThingstreamClient instance
 * @param topicName the name of the topic to subscribe to (can be wild-carded)
 * @param qos the #ThingstreamQualityOfService_t required
 * @param pOutTopic a pointer (may be NULL) to the ThingstreamTopic to receive the topic
 *        type and id of the named topic.
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_subscribeName(ThingstreamClient* client, const char* topicName, ThingstreamQualityOfService_t qos, ThingstreamTopic* pOutTopic);

/**
 * Subscribe to receive messages that are published on a topic.
 * When messages arrive the Thingstream_Application_subscribeCallback() will
 * be called with the topic, QoS, and message payload.
 *
 * @param client the #ThingstreamClient instance
 * @param topic the type and id of the #ThingstreamTopic to subscribe to
 * @param qos the #ThingstreamQualityOfService_t required
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_subscribeTopic(ThingstreamClient* client, ThingstreamTopic topic, ThingstreamQualityOfService_t qos);

/**
 * Unsubscribe and stop receiving messages published to a topic.
 * @param client the #ThingstreamClient instance
 * @param topicName the name of the topic to unsubscribe from
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_unsubscribeName(ThingstreamClient* client, const char *topicName);

/**
 * Unsubscribe and stop receiving messages published to a topic.
 * @param client the #ThingstreamClient instance
 * @param topic the type and id of the #ThingstreamTopic to unsubscribe from
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_unsubscribeTopic(ThingstreamClient* client, ThingstreamTopic topic);

/**
 * Process any outstanding tasks that are waiting.
 *
 * While the application is connected to the server (i.e. has successfully done
 * a Thingstream_Client_connect() but not yet done a Thingstream_Client_disconnect())
 * the application is expected to periodically call Thingstream_Client_run().
 *
 * If the server has sent a message to this device then Thingstream_Application_subscribeCallback()
 * will called from within this function to deliver the message to the application.
 *
 * @param client the #ThingstreamClient instance
 * @param waitMs the time in milliseconds to wait for things to happen,
 *        treat zero as "the smallest time that allows pending things to happen"
 * @return a #ThingstreamClientResult indicating success/fail
 */
extern ThingstreamClientResult Thingstream_Client_run(ThingstreamClient* client, uint32_t waitMs);


/**
 * Translate an error code into a concise representative string.
 * @param result a #ThingstreamClientResult code
 * @return text describing the result
 */
extern const char *Thingstream_Client_getErrorText(ThingstreamClientResult result);


/**
 * This string matches the Thingstream version e.g. BLD1234-v5.6
 */
extern const char Thingstream_Client_versionString[];


/**
 * This application supplied routine will be called when the Thingstream SDK
 * receives a time packet from the server.
 *
 * The server currently sends a time packet in response to the device calling:
 * - Thingstream_Client_ping()
 * - Thingstream_Client_connect()
 * - Thingstream_Client_publish()
 *
 * but time packets may be sent more frequently in the future.
 *
 * @ingroup porting-application
 * @param unixEpochTime the number of seconds since 00:00:00 UTC on 1 Jan 1970
 */
void Thingstream_Application_serverTimeCallback(uint32_t unixEpochTime);

/* Include backward compatibility APIs to set callbacks */
#include "client_set_callback.h"

#if defined(__cplusplus)
}
#endif

#endif /* INC_CLIENT_API_H_ */
