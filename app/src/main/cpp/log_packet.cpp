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

using json = nlohmann::json;
using namespace std;


#define  LOG_TAG    "witnettest"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

// // https://android.googlesource.com/kernel/msm.git/+/android-7.1.0_r0.3/drivers/char/diag/diag_dci.c
// функция diag_process_single_dci_pkt и его хедер
//#define LOG_CMD_CODE		0x10

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

    // Parse Header
    //vector<pair<string, string>> result;
    json j;

    offset = 0;
    offset += _decode_by_fmt(LogPacketHeaderFmt, ARRAY_SIZE(LogPacketHeaderFmt, Fmt),
                             b, offset, length, j["header"]);


    std::string s = j.dump();

    LOGD("json:  %s\n", s.c_str());


    // Differentiate using type ID
/*
    LogPacketType type_id = (LogPacketType) _map_result_field_to_name(
            result,
            "type_id",
            LogPacketTypeID_To_Name,
            ARRAY_SIZE(LogPacketTypeID_To_Name, ValueName),
            "Unsupported");

    if (skip_decoding) {    // skip further decoding

        PyObject *t = Py_BuildValue("(ss#s)",
                                    "Msg", b + offset, length-offset,
                                    "raw_msg/skip_decoding");
        PyList_Append(result, t);
        Py_DECREF(t);
        return result;
    }

    on_demand_decode(b+offset, length-offset, type_id, result);
*/
    return "just string";
}