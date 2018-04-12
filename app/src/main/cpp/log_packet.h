//
// Created by siralex on 11-Apr-18.
//

#ifndef WITNETTEST004_LOG_PACKET_H
#define WITNETTEST004_LOG_PACKET_H

#include <string>
#include <sstream>

// Field types
enum FmtType {
    UINT,                       // Little endian. len = 1, 2, 4, 8
    BYTE_STREAM,                // A stream of bytes.
    BYTE_STREAM_LITTLE_ENDIAN,  // a stream of bytes in little endian
    QCDM_TIMESTAMP,             // Timestamp in all messages. len = 8
    PLMN_MK1,                   // in WCDMA Cell ID
    PLMN_MK2,                   // in LTE NAS EMM State
    BANDWIDTH,                  // in LTE RRC Serving Cell Info, LTE RRC MIB Message
    RSRP,
    RSRQ,
    SKIP,                       // This field is ignored (but bytes are consumed)
    PLACEHOLDER,                // This field is created with a dummy value (no byte is consumed)
    WCDMA_MEAS,                 // Used for RSCP/RSSI/ECN0 in LTE_PHY_IRAT_MDB
};

struct Fmt {
    FmtType type;
    const char *field_name;
    int len;                    // Some FmtType has restrictions on this field.
};

const Fmt LogPacketHeaderFmt [] = {
        {SKIP, NULL, 2},
        {UINT, "log_msg_len", 2},
        {UINT, "type_id", 2},
        {QCDM_TIMESTAMP, "timestamp", 8}
};



bool is_log_packet (const char *b, size_t length);
std::string decode_log_packet (const char *b, size_t length, bool skip_decoding);






#endif //WITNETTEST004_LOG_PACKET_H
