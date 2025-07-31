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
 * @brief The API describing transport implementations
 *
 * Implementations of this API offer an implementation-specific function for
 * creating an instance of ThingstreamTransport. Note that implementations can choose
 * to return a single, statically allocated instance rather than allowing
 * multiple instances to exist in parallel.
 */

#ifndef INC_TRANSPORT_API_H_
#define INC_TRANSPORT_API_H_

#if defined(__cplusplus)
extern "C" {
#elif 0
}
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * A buffer of size THINGSTREAM_USSD_BUFFER_LEN is large enough to receive the
 * biggest packet that the server believes can be handled by typical modems.
 * The normal Thingstream modem transport will also use only this number of
 * bytes in the buffer obtained from the serial (or line buffer transport) so
 * using a larger value in the serial transport would just waste space.
 */
#define THINGSTREAM_USSD_BUFFER_LEN 145


/* The definition of ThingstreamResult is in thingstream_result.h */
#include "thingstream_result.h"

/**
 * The subset of #ThingstreamResult values that start with TRANSPORT_
 *
 * See #ThingstreamResult for details of individual values.
 */
typedef ThingstreamResult ThingstreamTransportResult;


/** @cond INTERNAL */

/**
 * @private
 * A macro to return true if the given #ThingstreamTransportResult is an error
 * that is specific to modem operation (e.g. an error from an AT command).
 * @param tRes the #ThingstreamTransportResult to test
 * @return true if tRes is an error that is specific to modem operation.
 */
#define THINGSTREAM_IS_MODEM_OPERATION_ERROR(tRes)                   \
    (((tRes) <= TRANSPORT_MODEM_ERROR)                               \
  && ((tRes) >= TRANSPORT_LAST_MODEM_ERROR))


/*
 * The version number is supplied in the low-order bits of the value
 * passed to the init entry point. Higher-order bits may be used
 * to pass information between transport modules during initialisation.
 */
#define TRANSPORT_VERSION_MASK              0x003f

/*
 * Higher-order bits are used as flag bits to pass information between
 * transport modules. The specific meanings of these bits is subject to
 * change between releases.
 */
#define TRANSPORT_VERSION_FLAG_CLIENT       0x0040
#define TRANSPORT_VERSION_FLAG_PROTOCOL     0x0080
#define TRANSPORT_VERSION_FLAG_BASE64       0x0100
#define TRANSPORT_VERSION_FLAG_DTLS         0x0200

/*
 * Including the enum size gives a cheap check that
 * toolchains for sdk and application are consistent.
 */
#define TRANSPORT_API_VERSION(v) \
     ((v) + ((sizeof(ThingstreamTransportResult) - 1) << 4))

/** @endcond */

/**
 * ThingstreamTransport API version. A version number is passed to
 * the init entry point to allow a runtime check that sources compiled
 * at different times are compatible.
 *
 * Source files should use the specific version of the API they
 * are written to.
 * @hideinitializer
 */
#define TRANSPORT_VERSION_1  TRANSPORT_API_VERSION(1)

/**
 * @deprecated
 * This is the symbol used in the example serial driver in older releases.
 */
#define TRANSPORT_VERSION    TRANSPORT_VERSION_1


/**
 * Check that the version number supplied is as expected
 * @hideinitializer
 */
#define TRANSPORT_CHECK_VERSION_1(passed)       \
    (( (passed) & TRANSPORT_VERSION_MASK) == TRANSPORT_VERSION_1)

/**
 * Type representing a transport instance.
 */
typedef struct ThingstreamTransport ThingstreamTransport;

/**
 * Type definition for the raw message received callback.
 * When the transport-specific code discovers a new message has arrived (either
 * by polling or using an interrupt or similar) the callback function will be
 * called to pass the inbound raw data to the next outermost ThingstreamTransport.
 * @param cookie a pointer to the user supplied cookie
 * @param data a pointer to the data
 * @param len the length of the data
 */
typedef void (*ThingstreamTransportCallback_t)(void* cookie, uint8_t* data, uint16_t len);

/**
 * Opaque type representing the internal state of a transport instance.
 */
typedef struct ThingstreamTransportState_s ThingstreamTransportState_t;

/**
 * Type definition for a transport instance.
 */
struct ThingstreamTransport {
    /**
     * Internal representation of the transport state. For portability,
     * ThingstreamTransport implementations should not access this directly, but rather
     * cast it to their concrete type definitions.
     */
    ThingstreamTransportState_t* _state;

    /**
     * Initialize the transport.
     * This may involve the setup on GPIO, UART ports, interrupts and other
     * platform dependencies.
     * @param version the transport API version
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*init)(ThingstreamTransport* self, uint16_t version);

    /**
     * Shutdown the transport (i.e. the opposite of initialize)
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*shutdown)(ThingstreamTransport* self);

    /**
     * Obtain a buffer to write data into.
     * @param buffer where to write the buffer pointer
     * @param len where the write the buffer length
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*get_buffer)(ThingstreamTransport* self, uint8_t** buffer, uint16_t* len);

    /**
     * @deprecated
     * Slot no longer used.
     */
    const char* (*unused_slot_1)(ThingstreamTransport* self);

    /**
     * Send the data to the next innermost ThingstreamTransport or hardware device.
     *
     * @param flags an indication of the type of the data, zero is normal.
     * @param data a pointer to the data
     * @param len the length of the raw data
     * @param millis the maximum number of milliseconds to run
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*send)(ThingstreamTransport* self, uint16_t flags, uint8_t* data, uint16_t len, uint32_t millis);

    /**
     * Register a callback function that will be called when this transport
     * has data to send to its next outermost ThingstreamTransport.
     *
     * @param callback the callback function
     * @param cookie a opaque value passed to the callback function
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*register_callback)(ThingstreamTransport* self, ThingstreamTransportCallback_t callback, void* cookie);

    /**
     * @deprecated
     * Slot no longer used.
     */
    ThingstreamTransportResult (*unused_slot_2)(ThingstreamTransport* self);

    /**
     * Allow the transport instance to run for at most the given number of
     * milliseconds.
     * @param millis the maximum number of milliseconds to run (a value of zero
     *        processes all pending operations).
     * @return a #ThingstreamTransportResult status code (success / fail)
     */
    ThingstreamTransportResult (*run)(ThingstreamTransport* self, uint32_t millis);
};

/** This function is used by a number of transport loggers */
typedef int (*ThingstreamPrintf_t)(const char* format, ...);

/* And these trace log bits are passed to the logger create apis */
/** enable tracing */
#define TLOG_TRACE    (1<<0)
/** enable verbose tracing */
#define TLOG_VERBOSE  (1<<1)
/** enable protocol tracing */
#define TLOG_PROTOCOL (1<<2)
/** add timestamps to start of log lines */
#define TLOG_TIME     (1<<3)

/* The TSEND_xxx bits can be used to pass extra info with transport->send(). */

/** packet being sent requires additional UserAgent block
 * (valid for use between client layer and thingstream transport only).
 */
#define TSEND_NEED_USERAGENT     (1U << 15)

/** a USSD disconnect is needed
 * (valid for use between thingstream, base64 and modem transports only).
 */
#define TSEND_USSD_SESSION_END   (1U << 14)

/** Bit (1U << 13) is no longer used. */

/** packet being sent would like additional GsmBearer block if the information
 * is available.
 * (valid for use between client layer and thingstream transport only).
 */
#define TSEND_WANT_GSM_BEARER    (1U << 12)

/** packet being sent would like additional BearerIndicator block
 * (valid for use between client layer and thingstream transport only).
 */
#define TSEND_WANT_BEARER_INDICATOR    (1U << 11)

#if defined(__cplusplus)
}
#endif

#endif /* INC_TRANSPORT_API_H_ */
