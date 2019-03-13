//
//  qcloud_live_net.h
//  saturn
//
//  Created by shockcao on 17/5/9.
//
//

#ifndef qcloud_live_net_h
#define qcloud_live_net_h

#include <string>

namespace qcloud {

    #define LIVE_QUIC_HOST "gbn.tim.qq.com"

    #define QUICTAG(a, b, c, d) \
    static_cast<uint32_t>((d << 24) + (c << 16) + (b << 8) + a)

    const uint32_t kTBBR = QUICTAG('T', 'B', 'B', 'R');
    const uint32_t kNSTP = QUICTAG('N', 'S', 'T', 'P');   // No stop waiting frames.
    const uint32_t kBYTE = QUICTAG('B', 'Y', 'T', 'E');
    const uint32_t kQBIC = QUICTAG('Q', 'B', 'I', 'C');   // TCP cubic
    const uint32_t kTLPR = QUICTAG('T', 'L', 'P', 'R');

    typedef bool (*LogMessageHandlerFunction)(int severity,
                                              const char* file, int line, size_t message_start, const std::string& str);

    enum CONNECT_STATE{
        kConnectIdle = 0,
        kConnecting,
        kConnected,
        kConnectFailed,
        kDisconnected,
    };

    enum LogLevel {
        QLOG_VERBOSE = -1,
        QLOG_INFO = 0,
        QLOG_WARNING = 1,
        QLOG_ERROR = 2,
        QLOG_FATAL = 3,
    };

    enum ErrorSource {
        FROM_SELF = 1,
        FROM_PEER = 2
    };

    enum NetErrorCode {
        ERR_NO = 0,
        ERR_IO_PENDING = -1,
        ERR_FAILED = -2,
        ERR_ABORTED = -3,
        ERR_CONNECTION_CLOSED = -100,
        ERR_CONNECTION_RESET = -101,
        ERR_WRITE_WHEN_CLOSED = -1,
        ERR_READ_WHEN_CLOSED = -1,
        ERR_STREAM_CLOSE_BEFORE_CONNCLOSE = -105,
    };

    enum SyncClientErrorCode {
        QUIC_SYNC_CONNTIMEOUT = -3,
        QUIC_SYNC_READTIMEOUT = -3,
        QUIC_SYNC_WRITETIMEOUT = -3,
        QUIC_SYNC_WRONGFORMAT = -2,
        QUIC_DISCONNECTED = -1,
    };

    enum QuicErrorCode {
        QUIC_NO_ERROR = 0,
        QUIC_INTERNAL_ERROR = 1,
        QUIC_STREAM_DATA_AFTER_TERMINATION = 2,
        QUIC_INVALID_PACKET_HEADER = 3,
        QUIC_INVALID_FRAME_DATA = 4,
        QUIC_MISSING_PAYLOAD = 48,
        QUIC_INVALID_FEC_DATA = 5,
        QUIC_INVALID_STREAM_DATA = 46,
        QUIC_OVERLAPPING_STREAM_DATA = 87,
        QUIC_UNENCRYPTED_STREAM_DATA = 61,
        QUIC_ATTEMPT_TO_SEND_UNENCRYPTED_STREAM_DATA = 88,
        QUIC_MAYBE_CORRUPTED_MEMORY = 89,
        QUIC_UNENCRYPTED_FEC_DATA = 77,
        QUIC_INVALID_RST_STREAM_DATA = 6,
        QUIC_INVALID_CONNECTION_CLOSE_DATA = 7,
        QUIC_INVALID_GOAWAY_DATA = 8,
        QUIC_INVALID_WINDOW_UPDATE_DATA = 57,
        QUIC_INVALID_BLOCKED_DATA = 58,
        QUIC_INVALID_STOP_WAITING_DATA = 60,
        QUIC_INVALID_PATH_CLOSE_DATA = 78,
        QUIC_INVALID_ACK_DATA = 9,
        QUIC_INVALID_VERSION_NEGOTIATION_PACKET = 10,
        QUIC_INVALID_PUBLIC_RST_PACKET = 11,
        QUIC_DECRYPTION_FAILURE = 12,
        QUIC_ENCRYPTION_FAILURE = 13,
        QUIC_PACKET_TOO_LARGE = 14,
        QUIC_PEER_GOING_AWAY = 16,
        QUIC_INVALID_STREAM_ID = 17,
        QUIC_INVALID_PRIORITY = 49,
        QUIC_TOO_MANY_OPEN_STREAMS = 18,
        QUIC_TOO_MANY_AVAILABLE_STREAMS = 76,
        QUIC_PUBLIC_RESET = 19,
        QUIC_INVALID_VERSION = 20,
        QUIC_INVALID_HEADER_ID = 22,
        QUIC_INVALID_NEGOTIATED_VALUE = 23,
        QUIC_DECOMPRESSION_FAILURE = 24,
        QUIC_NETWORK_IDLE_TIMEOUT = 25,
        QUIC_HANDSHAKE_TIMEOUT = 67,
        QUIC_ERROR_MIGRATING_ADDRESS = 26,
        QUIC_ERROR_MIGRATING_PORT = 86,
        QUIC_PACKET_WRITE_ERROR = 27,
        QUIC_PACKET_READ_ERROR = 51,
        QUIC_EMPTY_STREAM_FRAME_NO_FIN = 50,
        QUIC_INVALID_HEADERS_STREAM_DATA = 56,
        QUIC_HEADERS_STREAM_DATA_DECOMPRESS_FAILURE = 97,
        QUIC_FLOW_CONTROL_RECEIVED_TOO_MUCH_DATA = 59,
        QUIC_FLOW_CONTROL_SENT_TOO_MUCH_DATA = 63,
        QUIC_FLOW_CONTROL_INVALID_WINDOW = 64,
        QUIC_CONNECTION_IP_POOLED = 62,
        QUIC_TOO_MANY_OUTSTANDING_SENT_PACKETS = 68,
        QUIC_TOO_MANY_OUTSTANDING_RECEIVED_PACKETS = 69,
        QUIC_CONNECTION_CANCELLED = 70,
        QUIC_BAD_PACKET_LOSS_RATE = 71,
        QUIC_PUBLIC_RESETS_POST_HANDSHAKE = 73,
        QUIC_FAILED_TO_SERIALIZE_PACKET = 75,
        QUIC_TOO_MANY_RTOS = 85,
        QUIC_HANDSHAKE_FAILED = 28,
        QUIC_CRYPTO_TAGS_OUT_OF_ORDER = 29,
        QUIC_CRYPTO_TOO_MANY_ENTRIES = 30,
        QUIC_CRYPTO_INVALID_VALUE_LENGTH = 31,
        QUIC_CRYPTO_MESSAGE_AFTER_HANDSHAKE_COMPLETE = 32,
        QUIC_INVALID_CRYPTO_MESSAGE_TYPE = 33,
        QUIC_INVALID_CRYPTO_MESSAGE_PARAMETER = 34,
        QUIC_INVALID_CHANNEL_ID_SIGNATURE = 52,
        QUIC_CRYPTO_MESSAGE_PARAMETER_NOT_FOUND = 35,
        QUIC_CRYPTO_MESSAGE_PARAMETER_NO_OVERLAP = 36,
        QUIC_CRYPTO_MESSAGE_INDEX_NOT_FOUND = 37,
        QUIC_UNSUPPORTED_PROOF_DEMAND = 94,
        QUIC_CRYPTO_INTERNAL_ERROR = 38,
        QUIC_CRYPTO_VERSION_NOT_SUPPORTED = 39,
        QUIC_CRYPTO_HANDSHAKE_STATELESS_REJECT = 72,
        QUIC_CRYPTO_NO_SUPPORT = 40,
        QUIC_CRYPTO_TOO_MANY_REJECTS = 41,
        QUIC_PROOF_INVALID = 42,
        QUIC_CRYPTO_DUPLICATE_TAG = 43,
        QUIC_CRYPTO_ENCRYPTION_LEVEL_INCORRECT = 44,
        QUIC_CRYPTO_SERVER_CONFIG_EXPIRED = 45,
        QUIC_CRYPTO_SYMMETRIC_KEY_SETUP_FAILED = 53,
        QUIC_CRYPTO_MESSAGE_WHILE_VALIDATING_CLIENT_HELLO = 54,
        QUIC_CRYPTO_UPDATE_BEFORE_HANDSHAKE_COMPLETE = 65,
        QUIC_CRYPTO_CHLO_TOO_LARGE = 90,
        QUIC_VERSION_NEGOTIATION_MISMATCH = 55,
        QUIC_BAD_MULTIPATH_FLAG = 79,
        QUIC_MULTIPATH_PATH_DOES_NOT_EXIST = 91,
        QUIC_MULTIPATH_PATH_NOT_ACTIVE = 92,
        QUIC_IP_ADDRESS_CHANGED = 80,
        QUIC_CONNECTION_MIGRATION_NO_MIGRATABLE_STREAMS = 81,
        QUIC_CONNECTION_MIGRATION_TOO_MANY_CHANGES = 82,
        QUIC_CONNECTION_MIGRATION_NO_NEW_NETWORK = 83,
        QUIC_CONNECTION_MIGRATION_NON_MIGRATABLE_STREAM = 84,
        QUIC_TOO_MANY_FRAME_GAPS = 93,
        QUIC_STREAM_SEQUENCER_INVALID_STATE = 95,
        QUIC_TOO_MANY_SESSIONS_ON_SERVER = 96,
        QUIC_LAST_ERROR = 98,
        QUIC_TASK_TIMEOUT = 100,
        QUIC_FIN_SENT = 101,
    };

}

#endif /* qcloud_live_net_h */

