//
// Created by siralex on 11-Apr-18.
//

#ifndef WITNETTEST004_LOG_PACKET_H
#define WITNETTEST004_LOG_PACKET_H

#include <string>
#include <sstream>
#include "consts.h"

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




const ValueName ValueNameCellIndex [] = {
        // 4 bits
        {0, "PCell"},
        {1, "SCell"},
};

const ValueName ValueNameBandClassGSM [] = {
        // 4 bits
        {0, "Current 900/1800 Setting"},
        {10, "1900 PCS"},
        {11, "GSM 850"},
};

const ValueName ValueNameTimerState [] = {
        // 8 bits
        {0, "Stopped"},
        {1, "Running"},
        {2, "Expired"},
};

const ValueName ValueNameEnableOrDisable [] = {
        {0, "Disabled"},
        {1, "Enabled"},
};

const ValueName ValueNameYesOrNo [] = {
        {0, "No"},
        {1, "Yes"},
};



const Fmt WcdmaCellIdFmt [] = {
        {UINT, "Uplink RF channel number", 4},    //Uplink RF channel number
        {UINT, "Download RF channel number", 4},    //Download RF channel number
        {UINT, "Cell ID", 4},               //Cell ID
        {UINT, "UTRA registration area (overlapping URAs)", 1}, // UTRA registration area (overlapping URAs)
        {SKIP, NULL, 2},    // Unknown yet
        {UINT, "Allowed call access", 1},   //Allowed call access
        {UINT, "PSC", 2},   //PSC
        {PLMN_MK1, "PLMN", 6},  //PLMN
        {UINT, "LAC", 4},    //Location area code
        {UINT, "RAC", 4}     //routing area code
};

bool is_log_packet (const char *b, size_t length);
std::string decode_log_packet (const char *b, size_t length, bool skip_decoding);






#endif //WITNETTEST004_LOG_PACKET_H
