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
 * @brief The Thingstream Result values from client and transport apis.
 */

#ifndef INC_THINGSTREAM_RESULT_H_
#define INC_THINGSTREAM_RESULT_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/** Enumeration of Thingstream result values.
 */
typedef enum ThingstreamResult_e
{
    /** The successful client API response */
    CLIENT_SUCCESS                        = 0,
    /** Transport Operation completed successfully. */
    TRANSPORT_SUCCESS                     =  0,

    /** @private The current command is being retried (async API only) */
    CLIENT_COMMAND_RETRY                  = -1,
    /** @private The current command is being processed (async API only) */
    CLIENT_COMMAND_IN_PROGRESS            = -2,

    /** The data supplied to Thingstream_Client_publish() is too long to be
     * transmitted.
     */
    CLIENT_PUBLISH_TOO_LONG               = -3,

    /** The Thingstream_Client_xxx() API timed out while waiting for the API
     * to complete.
     */
    CLIENT_OPERATION_TIMED_OUT            = -5,
    /** The server rejected the domainKey that was passed to
     * Thingstream_Client_connect()
     */
    CLIENT_DOMAIN_KEY_INVALID             = -6,
    /** @deprecated renamed to #CLIENT_DOMAIN_KEY_INVALID */
    CLIENT_CLIENT_ID_INVALID = CLIENT_DOMAIN_KEY_INVALID,
    /** The client API could not be completed as the client is not connected.\n
     * The application should call Thingstream_Client_connect() to establish a
     * connection with the server.
     */
    CLIENT_NOT_CONNECTED                  = -10,
    /** A feature is not implemented in this version of the SDK */
    CLIENT_FEATURE_NOT_IMPLEMENTED        = -11,
    /** The client API was supplied with an illegal argument */
    CLIENT_ILLEGAL_ARGUMENT               = -12,
    /** The MQTTSN packet received from the server could not be decoded */
    CLIENT_MQTTSN_DECODE_ERROR            = -13,
    /** The server reports that the topic was invalid (e.g. contains illegal
     * characters or no predefined topic with that alias was found).
     */
    CLIENT_TOPIC_INVALID                  = -14,
    /** The server reports severe congestion and requests a later retry */
    CLIENT_CONGESTION                     = -15,
    /** The SDK state machine received responses that do not match with
     * expected state transitions.
     */
    CLIENT_WRONG_STATE                    = -16,
    /** Thingstream_Client_connect() received an unexpected response when
     * waiting for acknowledgment.
     */
    CLIENT_CONNECT_BAD_ACK                = -18,
    /** Thingstream_Client_register() received an unexpected response when
     * waiting for acknowledgment.
     */
    CLIENT_REGISTER_BAD_ACK               = -19,
    /** Thingstream_Client_publish() received an unexpected response when
     * waiting for acknowledgment.
     */
    CLIENT_PUBLISH_BAD_ACK                = -20,
    /** Thingstream_Client_subscribeName() or
     * Thingstream_Client_subscribeTopic() received an unexpected response when
     * waiting for acknowledgment.
     */
    CLIENT_SUBSCRIBE_BAD_ACK              = -21,
    /** Thingstream_Client_unsubscribeName() or
     * Thingstream_Client_unsubscribeTopic() received an unexpected response
     * when waiting for acknowledgment.
     */
    CLIENT_UNSUBSCRIBE_BAD_ACK            = -22,

    /** The information requested is not available */
    CLIENT_INFORMATION_NOT_AVAILABLE      = -23,

    /** Recursive call is not permitted */
    CLIENT_RECURSIVE_CALL                 = -24,

    /*
     * Add extra ClientResult error values before
     * CLIENT_UNKNOWN_TRANSPORT_ERROR which must be the last CLIENT_XXX error.
     */

    /** An out-of-range result value was returned by a transport api */
    CLIENT_UNKNOWN_TRANSPORT_ERROR        = -39,

    /** Transport Operation failed with an unspecified error. */
    TRANSPORT_ERROR                       = -40,
    /** Transport initialized with wrong version. */
    TRANSPORT_VERSION_MISMATCH            = -41,
    /** Transport encountered unexpected data. */
    TRANSPORT_UNEXPECTED_DATA             = -42,
    /** Transport Read operation timed out. */
    TRANSPORT_INIT_TIMEOUT                = -43,
    /** Transport Read operation timed out. */
    TRANSPORT_READ_TIMEOUT                = -44,
    /** Transport Read operation overflowed the available buffer. */
    TRANSPORT_READ_OVERFLOW               = -45,
    /** Transport Send operation timed out. */
    TRANSPORT_SEND_TIMEOUT                = -46,
    /** Transport Illegal argument or parameter */
    TRANSPORT_ILLEGAL_ARGUMENT            = -47,
    /** Transport Internal protocol error */
    TRANSPORT_INTERNAL_ERROR              = -48,
    /** Transport Send operation timed out waiting for ACK. */
    TRANSPORT_ACK_TIMEOUT                 = -49,
    /** Transport Send operation timed out waiting for ACK between packets */
    TRANSPORT_INTERPACKET_ACK_TIMEOUT     = -50,
    /** Transport Send operation timed out waiting for END which had been deferred
     * from a previous ->send() operation.
     */
    TRANSPORT_DEFERRED_END_TIMEOUT        = -51,
    /** Transport Send operation timed out waiting for END */
    TRANSPORT_END_TIMEOUT                 = -52,
    /** Transport modem->init() timed out waiting for OK after initial AT command */
    TRANSPORT_INIT_AT_FAILURE             = -53,
    /** Transport modem->init() timed out waiting for +CREG:5 */
    TRANSPORT_INIT_CREG5_TIMEOUT          = -54,
    /** Transport modem->init() registration refused */
    TRANSPORT_INIT_CREG_REFUSED           = -55,
    /** Transport buffer too small */
    TRANSPORT_BUFFER_TOO_SMALL            = -56,
    /** Transport modem->init() unable to reset UDP network stack */
    TRANSPORT_INIT_UDP_NETWORK_RESET_FAILED = -57,
    /** Transport modem->init() unable to open the UDP network stack */
    TRANSPORT_INIT_UDP_NETWORK_OPEN_FAILED = -58,
    /** Transport modem->init() unable to attach UDP context to network stack */
    TRANSPORT_INIT_UDP_NETWORK_ATTACH_FAILED = -59,
    /** Transport modem->init() unable to set the required APN in the modem */
    TRANSPORT_INIT_APN_SETUP_FAILED       = -60,
    /** Transport modem->init() unable to obtain local IP address */
    TRANSPORT_INIT_UDP_IP_ADDR_FAILED     = -61,
    /** Transport modem->init() unable to open UDP connection to the server */
    TRANSPORT_INIT_UDP_CONNECT_FAILED     = -62,
    /** Transport modem->init() unable to create a UDP socket */
    TRANSPORT_INIT_UDP_SOCKET_CREATE_FAILED = -63,

    /** Transport Modem operation failed with an unspecified error. */
    TRANSPORT_MODEM_ERROR                 = -64,
    /** Transport Modem operation failed with an unspecified CME error. */
    TRANSPORT_MODEM_CME_ERROR             = -65,
    /** Transport Modem operation failed and the modem driver forced a soft reset. */
    TRANSPORT_MODEM_FORCED_RESET          = -66,
    /** Transport Modem operation failed with an +CUSD:2 error. */
    TRANSPORT_MODEM_CUSD2_ERROR           = -67,
    /** Transport Modem operation failed with an +CUSD:3 error. */
    TRANSPORT_MODEM_CUSD3_ERROR           = -68,
    /** Transport Modem operation failed with an +CUSD:4 error. */
    TRANSPORT_MODEM_CUSD4_ERROR           = -69,
    /** Transport Modem operation failed with an +CUSD:5 error. */
    TRANSPORT_MODEM_CUSD5_ERROR           = -70,

    /**
     * @private
     * This is used by the #THINGSTREAM_IS_MODEM_OPERATION_ERROR() macro.
     */
    TRANSPORT_LAST_MODEM_ERROR            = TRANSPORT_MODEM_CUSD5_ERROR,


    /** Transport Modem (dual udp/ussd) used with line (not ring) buffer transport. */
    TRANSPORT_MODEM_WITH_LINE_BUFFER_ERROR   = -71,
    /** Transport Modem ussd requires base64 in the transport stack. */
    TRANSPORT_MODEM_USSD_BASE64_ERROR     = -72,
    /** Transport Modem has rejected the send request and no data was sent. */
    TRANSPORT_MODEM_SEND_RETRY_REQUEST    = -73,
    /** Transport stack contains mutually incompatible modules */
    TRANSPORT_INVALID_STACK               = -74,
    /** Thingstream server reports a protocol error */
    TRANSPORT_ERROR_FROM_SERVER           = -75,

    /** All error values from Thingstream_Client_xxx() APIs will be greater
     * than #CLIENT_MAX_ERROR and less than #CLIENT_SUCCESS
     */
    CLIENT_MAX_ERROR                      = -127
} ThingstreamResult;


#if defined(__cplusplus)
}
#endif

#endif /* INC_TRANSPORT_RESULT_H_ */
