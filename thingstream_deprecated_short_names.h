/*
 * Copyright 2017-2023 Thingstream AG
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
 * @brief **Deprecated** Thingstream short name symbol mappings
 */

#ifndef INC_THINGSTREAM_DEPRECATED_SHORT_NAMES_H_
#define INC_THINGSTREAM_DEPRECATED_SHORT_NAMES_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include "client_api.h"

/**
 * @addtogroup legacy-mappings
 * @{
 */

/** @deprecated Alias for #ThingstreamClient */
typedef ThingstreamClient Client;

/** @deprecated Alias for #ThingstreamTopic */
typedef ThingstreamTopic  Topic;

/** @deprecated Alias for #ThingstreamClientResult */
typedef ThingstreamClientResult ClientResult;

/** @deprecated Alias for #ThingstreamQualityOfService_t */
typedef ThingstreamQualityOfService_t QOS;

/** @deprecated Alias for #ThingstreamQOS0 */
#define MQTT_QOS0          ThingstreamQOS0

/** @deprecated Alias for #ThingstreamQOS1 */
#define MQTT_QOS1          ThingstreamQOS1

/** @deprecated Alias for #ThingstreamQOS2 */
#define MQTT_QOS2          ThingstreamQOS2

/** @deprecated Alias for #ThingstreamQOSM1 */
#define MQTTSN_QOSM1       ThingstreamQOSM1

/**
 * The duration (in minutes) that the server will consider the connection
 * active after receiving a message from the client.
 */
extern uint16_t Thingstream_Client_connectionTimeout;

/**
 * @private
 * A place to store the domain key between Client_create() and Client_connect()
 */
extern char Thingstream_Client__domainKey[];

/**
 * @private implementation of the legacy Client_create()
 */
extern ThingstreamClient* Thingstream__deprecatedCreateClient(ThingstreamTransport * transport, const char* domainKey);

/** @deprecated
 * Split into two APIs Thingstream_createClient() and Thingstream_Client_init()
 * @param transport the #ThingstreamTransport instance
 * @param domainKey a unique identifier for the client (use NULL to obtain
 *        a value from the transport layer).
 * @return a pointer to a #ThingstreamClient instance, or NULL if the client instance
 *         cannot be created and initialised.
 * @hideinitializer
 */
#define Client_create(transport, domainKey)      Thingstream__deprecatedCreateClient(transport, domainKey)


/** @deprecated
 * Split into two APIs Thingstream_Client_shutdown() and Thingstream_Client_destroy()
 * but since the latter is currently a no-op, this is just mapped to the former.
 */
#define Client_destroy     Thingstream_Client_shutdown

/** @deprecated renamed to Thingstream_Client_disconnect() */
#define Client_disconnect  Thingstream_Client_disconnect

/** @deprecated renamed to Thingstream_Client_ping() */
#define Client_isConnected Thingstream_Client_ping

/** @deprecated renamed to Thingstream_Client_register() */
#define Client_register    Thingstream_Client_register

/** @deprecated       renamed to Thingstream_Client_subscribeName() */
#define Client_subscribeName     Thingstream_Client_subscribeName

/** @deprecated       renamed to Thingstream_Client_subscribeTopic() */
#define Client_subscribeTopic    Thingstream_Client_subscribeTopic

/** @deprecated       renamed to Thingstream_Client_unsubscribeName() */
#define Client_unsubscribeName   Thingstream_Client_unsubscribeName

/** @deprecated       renamed to Thingstream_Client_unsubscribeTopic() */
#define Client_unsubscribeTopic  Thingstream_Client_unsubscribeTopic

/** @deprecated       renamed to Thingstream_Client_run() */
#define Client_run               Thingstream_Client_run

/** @deprecated       renamed to Thingstream_Client_getErrorText() */
#define Client_getErrorText      Thingstream_Client_getErrorText

/** @deprecated
 * Renamed to Thingstream_Client_connect() with
 * additional parameters to set the timeout (in place of #Client_ConnectionTimeout),
 * and to take the domain key (previously passed to Client_create())
 * The callback and cookie parameters should instead be passed to a separate call to
 * Thingstream_Client_setRegisterCallback().
 */
#define Client_connect(client, clean, callback, cookie) \
    ( Thingstream_Client_setRegisterCallback((client), (callback), (cookie)), \
      Thingstream_Client_connect((client), (clean), Thingstream_Client_connectionTimeout, Thingstream_Client__domainKey) )

/** @deprecated renamed to Thingstream_Client_publish() without the final parameter */
#define Client_publish(client, topic, qos, retained, payload, payloadlen, unused) \
    Thingstream_Client_publish((client), (topic), (qos), (retained), (payload), (payloadlen))

/**
 * @deprecated This is used by the compatabilty macro Client_connect() but
 * not when Thingstream_Client_connect() is used.
 */
#define Client_ConnectionTimeout Thingstream_Client_connectionTimeout

/** @deprecated renamed to #Thingstream_PredefinedSelfTopic */
#define PredefinedSelfTopic Thingstream_PredefinedSelfTopic

/** @deprecated Alias for #ThingstreamRegisterCallback_t */
#define Register_callback ThingstreamRegisterCallback_t

/** @deprecated Alias for #ThingstreamSubscribeCallback_t */
#define Subscribe_callback ThingstreamSubscribeCallback_t

/** @deprecated              renamed to Thingstream_Client_setSubscribeCallback() */
#define Client_set_subscribe_callback   Thingstream_Client_setSubscribeCallback

/** @deprecated              renamed to Thingstream_Client_clearSubscribeCallback() */
#define Client_clear_subscribe_callback Thingstream_Client_clearSubscribeCallback

/** @deprecated              renamed to Thingstream_Client_setRegisterCallback() */
#define Client_set_register_callback    Thingstream_Client_setRegisterCallback

/** @deprecated             renamed to Thingstream_Client_clearRegisterCallback() */
#define Client_clear_register_callback Thingstream_Client_clearRegisterCallback

/** @deprecated         renamed to Thingstream_Platform_getTimeMillis() */
#define Platform_getTimeMillis     Thingstream_Platform_getTimeMillis

/** @deprecated         renamed to Thingstream_Platform_getTimeString() */
#define Platform_getTimeString     Thingstream_Platform_getTimeString

/** @deprecated                renamed to Thingstream_createProtocolLogger() */
#define log_thingstream_transport_create  Thingstream_createProtocolLogger

/** @deprecated          renamed to Thingstream_createModemLogger() */
#define log_modem_transport_create  Thingstream_createModemLogger

/** @deprecated             renamed to Thingstream_createProtocolTransport() */
#define thingstream_transport_create   Thingstream_createProtocolTransport

/** @deprecated                           renamed to Thingstream_Protocol_setUssdSessionDelay() */
#define thingstream_transport_set_ussd_session_delay Thingstream_Protocol_setUssdSessionDelay

/** @deprecated        renamed to #Thingstream_Modem_initString */
#define ModemInitString            Thingstream_Modem_initString

/** @deprecated        renamed to #Thingstream_Modem_informationString */
#define ModemInformationString     Thingstream_Modem_informationString

/** @deprecated        renamed to #Thingstream_Modem_ussdEndSessionString */
#define ModemUssdEndSessionString  Thingstream_Modem_ussdEndSessionString

/** @deprecated        renamed to #Thingstream_Modem_forceResetString */
#define ModemForceResetString      Thingstream_Modem_forceResetString

/** @deprecated Alias for #ThingstreamModemCallback_t */
#define Modem_callback ThingstreamModemCallback_t

/** @deprecated renamed to Thingstream_Modem_setCallback() */
#define Modem2_set_modem_callback   Thingstream_Modem_setCallback

/** @deprecated renamed to Thingstream_Modem_clearCallback() */
#define Modem2_clear_modem_callback Thingstream_Modem_clearCallback

/** @deprecated   renamed to Thingstream_createBase64CodecTransport() */
#define base64_codec_create  Thingstream_createBase64CodecTransport

/** @deprecated           renamed to Thingstream_createClientLogger() */
#define log_client_transport_create  Thingstream_createClientLogger

/** @deprecated               renamed to Thingstream_createLineBufferTransport() with an extra argument */
#define line_buffer_transport_create Thingstream__createLegacyLineBufferTransport

/** @deprecated renamed to Thingstream_createModem2Transport()
 * Note that Thingstream_createModem2Transport() is itself renamed.
 */
#define modem2_transport_create     Thingstream_createModem2Transport

/** @deprecated renamed to Thingstream_Modem_sendLine() */
#define Modem2_send_line            Thingstream_Modem_sendLine

/** @deprecated renamed to Thingstream_Modem_getCUSDErrors() */
#define Modem2_get_CUSD_errors      Thingstream_Modem_getCUSDErrors

/** @deprecated renamed to Thingstream_Modem_getSeriousErrors() */
#define Modem2_get_serious_errors   Thingstream_Modem_getSeriousErrors

/** @deprecated Alias for #ThingstreamTransport */
typedef ThingstreamTransport Transport;

/** @deprecated Alias for #ThingstreamPrintf_t */
typedef ThingstreamPrintf_t transport_logger;

/** @deprecated Alias for #ThingstreamTransportResult */
typedef ThingstreamTransportResult TransportResult;

/** @deprecated Alias for #ThingstreamTransportCallback_t */
typedef ThingstreamTransportCallback_t Transport_callback;

/** @deprecated Alias for #ThingstreamTransportState_t */
typedef ThingstreamTransportState_t TransportState;

/** @deprecated         renamed to Thingstream_createCustomModemTransport() */
#define custom_modem_transport_create  Thingstream_createCustomModemTransport

/*! @} */


#if defined(__cplusplus)
}
#endif

#endif /* INC_THINGSTREAM_DEPRECATED_SHORT_NAMES_H_ */
