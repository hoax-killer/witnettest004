//
// Created by siralex on 27-Apr-18.
//

#ifndef WITNETTEST004_WCDMA_SIGNALING_MESSAGES_H
#define WITNETTEST004_WCDMA_SIGNALING_MESSAGES_H


#include "log_packet_utils.h"
#include "utils.h"
#include "consts.h"
#include "nlohmann/json.hpp"

const Fmt WcdmaSignalingMessagesFmt [] = {
        {UINT, "Channel Type", 1},
        {UINT, "Radio Bearer ID", 1},
        {UINT, "Message Length", 2}
};

const ValueName WcdmaSignalingMsgChannelType [] = {
        {0x00, "RRC_UL_CCCH"},
        {0x01, "RRC_UL_DCCH"},
        {0x02, "RRC_DL_CCCH"},
        {0x03, "RRC_DL_DCCH"},
        {0x04, "RRC_DL_BCCH_BCH"},
        {0x06, "RRC_DL_PCCH"},
        {0x09, "Extension SIB"},
        {0xfe, "RRC_COMPLETE_SIB"},
};



static int
_decode_wcdma_signaling_messages(const char *b, int offset, size_t length,
                                 json &j) {
    (void)length;

    int ch_num = j["payload"]["WcdmaSigMsgs"]["Channel Type"];

    const char *ch_name = search_name(WcdmaSignalingMsgChannelType,
                                      ARRAY_SIZE(WcdmaSignalingMsgChannelType, ValueName),
                                      ch_num);

    if (ch_name == NULL) {  // not found
        LOGD("Unknown WCDMA Signalling Messages Channel Type: 0x%x\n", ch_num);
        return 0;
    }

    int pdu_length = j["payload"]["WcdmaSigMsgs"]["Message Length"];

    //std::string raw_data (b + offset, pdu_length);
    std::string raw_data = hexStr((unsigned char *)b + offset, pdu_length);
    j["payload"]["WcdmaSigMsgs"]["raw msg"]["channel name"] = ch_name;
    j["payload"]["WcdmaSigMsgs"]["raw msg"]["msg length"] = pdu_length;
    j["payload"]["WcdmaSigMsgs"]["raw msg"]["msg data"] = raw_data;


    return pdu_length;
}
























#endif //WITNETTEST004_WCDMA_SIGNALING_MESSAGES_H
