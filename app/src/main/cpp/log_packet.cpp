//
// Created by siralex on 11-Apr-18.
//

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include "log_packet.h"
#include "log_packet_utils.h"
#include "utils.h"
#include "consts.h"
#include <android/log.h>
#include "nlohmann/json.hpp"
#include "gsm_rr_cell_reselection_meas.h"
#include "lte_phy_serving_cell_meas_res.h"

using json = nlohmann::json;
using namespace std;

void
payload_decode (const char *b, size_t length, LogPacketType type_id, json &j)
{
    //LOGD("in payload_decode\n");
    int offset = 0;
    json jj;
    switch (type_id) {
        case LTE_PHY_Serving_Cell_Measurement_Result: {
            offset += _decode_by_fmt(LtePhySubpktFmt,
                                     ARRAY_SIZE(LtePhySubpktFmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["LtePhySubpkt"]["header"] = jj;
            offset += _decode_lte_phy_subpkt(b, offset, length, j);

            break;
        }
        case GSM_RR_Cell_Reselection_Meas: {
            offset += _decode_by_fmt(GsmRrCellResMeas_Fmt,
                                     ARRAY_SIZE(GsmRrCellResMeas_Fmt, Fmt),
                                     b, offset, length, jj);
            j["payload"]["GsmRrCellResMeas"] = jj;
            string ss = j.dump();
            //LOGD("packet: %s\n", ss.c_str());
            offset += _decode_gsm_rcrm_payload(b, offset, length, j);
            break;
        }
       // case WCDMA_CELL_ID:
        //    offset += _decode_by_fmt(WcdmaCellIdFmt,
         //                            ARRAY_SIZE(WcdmaCellIdFmt, Fmt),
          //                           b, offset, length, result);
          //  break;
        default: {
            LOGD("unknown payload type\n");
        }
    };
}



bool
is_log_packet (const char *b, size_t length) {
    return length >= 2 && b[0] == '\x10';
}

bool
is_debug_packet (const char *b, size_t length) {
    return length >=2 && (b[0] ==  '\x79' || b[0] == '\x92');
    // return length >=2 && (b[0] == '\x92');  //Yuanjie: optimization for iCellular, avoid unuseful debug msg
    // return length >=2 && (b[0] ==  '\x79');
}


string decode_log_packet (const char *b, size_t length, bool skip_decoding) {

    int offset = 0;

    json j;

    offset = 0;
    offset += _decode_by_fmt(LogPacketHeaderFmt, ARRAY_SIZE(LogPacketHeaderFmt, Fmt),
                             b, offset, length, j["header"]);


    unsigned int type_id = j["header"]["type_id"];


    bool typeid_found = false;
    for (int i = 0; i < LogPacketTypeID_To_Name.size(); i++) {
        if(LogPacketTypeID_To_Name[i].val == type_id){
            typeid_found = true;
            break;
        }
    }


    if(typeid_found){
        payload_decode(b+offset, length-offset, (LogPacketType)type_id, j);
        return j.dump();
    }else{ // return header only
        return j.dump();
    }

}

