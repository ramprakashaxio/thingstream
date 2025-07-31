/*
 * Copyright 2017-2025 Thingstream AG
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
 * @ingroup udp
 * @brief A modem driver (implemented as a ThingstreamTransport instance) that
 * supports both USSD and UDP communication.
 *
 * This driver is used with a hardware specific modem configuration.
 * See the @ref udp_modems "list of supported modems".
 *
 * Note: the MODEM_PREFER_USSD flag is no longer supported.
 */

#ifndef INC_MODEM_TRANSPORT_H
#define INC_MODEM_TRANSPORT_H

#include <stdbool.h>
#include "transport_api.h"

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

/**
 * @anchor modemflags
 * @name Modem flags
 * The following flags can be passed to Thingstream_createModemTransport().
 * Multiple flags can be combined using bitwise OR.
 * @{
 */

/**
 * When this flag is set, the modem transport layer considers the modem
 * hardware to be initialised already and will skip its own initialisation.
 * This is needed when other features of the modem hardware are programmed by
 * non-Thingstream software.
 */
#define MODEM_SKIP_INIT  (0x01)


/**
 * The modem will usually talk to the normal (or "live") Thingstream software
 * server stack, but occasionally the Thingstream support team may request that
 * you use the debugging rather than live stack. Setting this flag tells the
 * modem transport layer to use the stage debugging stack.
 */
#define MODEM_STAGE_SHORTCODE  (0x02)


/**
 * The modem allows the AT+CUSD=2 command (which denotes the end of a USSD
 * session) to be combined with the last USSD message.
 *
 * This flag and #MODEM_SPLIT_USSD_SESSION_END are mutually exclusive.
 * Do not set both flags.
 * If neither flag is set, then one of them will be chosen during initialisation.
 */
#define MODEM_MERGE_USSD_SESSION_END (0x4)

/**
 * The modem requires the AT+CUSD=2 command (which denotes the end of a USSD
 * session) to be split from the last USSD message.
 *
 * This flag and #MODEM_MERGE_USSD_SESSION_END are mutually exclusive.
 * Do not set both flags.
 * If neither flag is set, then one of them will be chosen during initialisation.
 */
#define MODEM_SPLIT_USSD_SESSION_END (0x8)


/**
 * Some modems (e.g. Quectel UG96) return unsolicited "+CUSD: 2" when a USSD
 * session is terminated by the server, but most modems return this for various
 * USSD error conditions.
 * By default the modem  driver will treat an unsolicited "+CUSD: 2" as an error
 * condition.
 * Setting this flag causes unsolicited "+CUSD: 2" messages to be treated as
 * informational only.
 */
#define MODEM_IGNORE_PLUS_CUSD2  (0x10)

/*
 * MODEM_PREFER_USSD (0x100) is no longer supported.
 */

/**
 * If #MODEM_SKIP_INFO_INIT is in the Thingstream_createModemTransport() flags
 * then the modem driver will skip those commands in the init string that are
 * for diagnostics and information.
 */
#define MODEM_SKIP_INFO_INIT    (0x200)

/**
 * If #MODEM_LOG_PARSED is in the flags passed to
 * Thingstream_createModemTransport() then the modem driver will log items
 * that it successfully parsed from the bytes received from the underlying
 * modem hardware.
 *
 * This flag bit is ignored if the SDK has been built without the matching
 * support.
*/
#define MODEM_LOG_PARSED       (0x400)

/**
 * If #MODEM_LOG_PARSING is in the flags passed to
 * Thingstream_createModemTransport() then the modem driver will log parsing
 * steps of bytes received from the underlying modem hardware.
 *
 * Note that this produces a very large amount of logging.
 *
 * This flag bit is ignored if the SDK has been built without the matching
 * support.
 */
#define MODEM_LOG_PARSING      (0x800)

/**
 * If #MODEM_NO_NBIOT_EDIT is in the flags passed to
 * Thingstream_createModemTransport() then the modem driver will inhibit the
 * editing of the radio access technology to remove NB-IoT.
 */
#define MODEM_NO_NBIOT_EDIT   (0x1000)

/** @} */


/**
 * This application supplied routine will be called when the modem transport
 * receives:
 *
 * * any response during Thingstream_Modem_sendLine()
 * * or a line starting with +CSQ, +COPS, +CREG, +CGREG, +CEREG
 * * or any line that is not recognized by the modem driver.
 *
 * Note that any newline characters have been stripped from the response,
 * and that response[len] is a zero byte.
 *
 * It is not permitted to call any Thingstream_xxx() apis from within this
 * callback.
 *
 * @ingroup porting-application
 * @param response the modem response
 * @param len the length of the response
 */
void Thingstream_Application_modemCallback(const char* response, uint16_t len);


/** See modem_init_string.c */
extern const char Thingstream_Modem_initString[];

/** See modem_info_string.c */
extern const char Thingstream_Modem_informationString[];

/** See modem_ussd_end_string.c */
extern const char Thingstream_Modem_ussdEndSessionString[];

/** See modem_fplmn_string.c */
extern const char Thingstream_Modem_readFplmnString[];

/** See modem_fplmn_string.c */
extern const char Thingstream_Modem_clearFplmnString[];

/** See modem_reset_string.c */
extern const char Thingstream_Modem_forceResetString[];

struct ThingstreamModemSharedState_s;
struct ThingstreamModemUdpConfig_s;

/**
 * Type definition for the hardware specific configuration initialisation
 * routine. When calling Thingstream_createModemTransport() the application
 * must select the appropriate routine to match the hardware.\n
 * See the @ref udp_modems "list of modem initialisation routines".
 *
 * For other modems please contact services-support@u-blox.com
 */
typedef const struct ThingstreamModemUdpConfig_s* ThingstreamModemUdpInit
(
    uint32_t version,
    struct ThingstreamModemSharedState_s* gState
);

/**
 * Create an instance of the modem transport.
 * @param inner the inner transport instance to use
 * @param flags a set of control flags. See
 * \ref modemflags "modem flags"
 * Set flags to '0' to get the default settings.
 * @param buffer the transport buffer to be used by outer transports
 * @param bufSize the size of the transport buffer.\n
 *   When using UDP we suggest a buffer size of #MODEM_UDP_BUFFER_LEN\n
 *   but when only USSD the buffer size should be #MODEM_USSD_BUFFER_LEN.
 * @param udpConfigInit the initialisation routine for a particular udp modem.\n
 *   See list of @ref udp_modems "modem initialisation routines".
 * @return the modem transport instance
 */
extern ThingstreamTransport* Thingstream_createModemTransport
(
    ThingstreamTransport* inner,
    uint16_t flags,
    uint8_t* buffer,
    uint16_t bufSize,
    ThingstreamModemUdpInit udpConfigInit
);


/** @cond INTERNAL */

/* Space reserved from buffer for parsing modem messages */
#define MODEM__RESERVED_BUFFER 64

/** @endcond */

/**
 * Recommended buffer size for USSD-only sessions.
 * @hideinitializer
 */
#define MODEM_USSD_BUFFER_LEN (THINGSTREAM_USSD_BUFFER_LEN + MODEM__RESERVED_BUFFER)

/**
 * Recommended buffer size for UDP sessions.
 * @hideinitializer
 */
#define MODEM_UDP_BUFFER_LEN (1000 + MODEM__RESERVED_BUFFER)

/**
 * @addtogroup udp_modems
 * @{
 */

/**
 * Pass #Thingstream_UssdInit to Thingstream_createModemTransport()
 * to configure the modem driver to use USSD without any support for UDP.
 * @hideinitializer
 */
extern ThingstreamModemUdpInit Thingstream_UssdInit;

/** @} */

/**
 * Send the line to the modem and wait for an OK response.
 * The application can implement Thingstream_Application_modemCallback() to
 * receive any response from the modem caused by the sent line.
 *
 * @param self this instance of modem transport
 * @param line a null-terminated line to send to the modem ("\r\n" will be added)
 * @param millis the maximum number of milliseconds to run
 * @return an integer status code (success / fail)
 */
ThingstreamTransportResult Thingstream_Modem_sendLine(ThingstreamTransport* self, const char* line, uint32_t millis);

/**
 * @private
 *
 * It may be possible to use this to implement binary file transfers on some modems.
 * Contact services-support@u-blox.com for more details.
 * Note that the interface is subject to change in future releases.
 * Currently, the maximum count is restricted by the buffer size - TRANSPORT_ILLEGAL_ARGUMENT
 * is returned if it is too large.
 *
 * @param self this instance of modem transport
 * @param count the expected number of bytes
 * @param postfix a marker sequence to skip after the data, or NULL
 * @return an integer status code (success / fail)
 */
ThingstreamTransportResult Thingstream_Modem_binaryMode(ThingstreamTransport* self, uint16_t count, const char *postfix);

/**
 * Return the number of accumulated +CUSD: errors.
 *
 * @param self this Transport instance
 * @param andClear if non-zero, then clear the count.
 * @return the number of accumulated +CUSD: errors.
 */
uint32_t Thingstream_Modem_getCUSDErrors(ThingstreamTransport* self, uint32_t andClear);

/**
 * Return the number of accumulated serious errors (those that trigger
 * a modem reset to recover)
 *
 * @param self this Transport instance
 * @param andClear if non-zero, then clear the count.
 * @return the number of accumulated serious errors.
 */
uint32_t Thingstream_Modem_getSeriousErrors(ThingstreamTransport* self, uint32_t andClear);

/**
 * Set the maximum sector size for the UDP transmissions.
 * This must be called before the Thingstream stack is initialised.
 *
 * @param self this Transport instance
 * @param mss the maximum-sector-size for the UDP transmissions
 */
ThingstreamTransportResult Thingstream_Modem_setBearerMSS(ThingstreamTransport* self, uint16_t mss);

/**
 * This application supplied routine will be called when the modem transport
 * receives binary data. If the application does not provide this routine then
 * the binary data will be sent to the Thingstream_Application_modemCallback().
 *
 * This feature is not supported for all modems, contact
 * services-support@u-blox.com for more details.
 *
 * @ingroup porting-application
 * @param data the binary data
 * @param len the length of the binary data
 */
void Thingstream_Application_modemBinaryCallback(const uint8_t* data, uint16_t len);


/**
 * The function in this variable will be called if the modem transport
 * has been compiled to include additional code to log modem internal
 * operations. The information provided is only of use to Thingstream support.
 */
extern ThingstreamPrintf_t Thingstream_Modem_internalLogger;


/**
 * The symbol #Thingstream__dontUseLineBufferTransportWithModem2 is used for
 * link-time checking.
 *
 * If your application gets multiple definition (or duplicate symbol) errors
 * for this symbol then your application may have attempted to create an SDK
 * transport stack with both the modem transport and the line buffer transport.
 *
 * These two transports are incompatible so your application should be using
 * the ring buffer transport instead.
 *
 * If your application needs to link both modem transport and line buffer
 * transports (i.e. you are using line buffer outside the SDK transport stack)
 * then please use Thingstream_createLineBufferTransport() instead of the
 * legacy line_buffer_transport_create() api.
 *
 * @deprecated
 */
void Thingstream__dontUseLineBufferTransportWithModem2(void);

/* Include backward compatibility APIs to set callbacks */
#include "modem_set_callback.h"

/* Include backward compatibility for modem2 APIs */
#include "modem2_transport.h"

#if defined(__cplusplus)
}
#endif

#endif /* INC_MODEM_TRANSPORT_H */
