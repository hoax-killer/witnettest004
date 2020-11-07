//
// Created by siralex on 20-Apr-18.
//

#ifndef WITNETTEST004_LTE_PHY_SERVING_CELL_MEAS_RES_H
#define WITNETTEST004_LTE_PHY_SERVING_CELL_MEAS_RES_H
#include "log_packet_utils.h"
#include "utils.h"
#include "consts.h"
#include <android/log.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

const Fmt LtePhySubpktFmt_v1_Scmr_v19 [] = {
        {UINT, "E-ARFCN", 4},
        {UINT, "Num-of-cells", 2},
        {SKIP, NULL, 2},
        {UINT, "Physical Cell ID", 2},  // 9 bits
        {PLACEHOLDER, "Serving Cell Index", 0},    // 3 bits
        {PLACEHOLDER, "Is Serving Cell", 0},    // 1 bit
        {SKIP, NULL, 2},
        {UINT, "Current SFN", 2},   // 10 bits
        {PLACEHOLDER, "Current Subframe Number", 0},    // 4 bits
        {SKIP, NULL, 2},
        {SKIP, NULL, 4},    // Is Restricted, Cell Timing [0]
        {SKIP, NULL, 4},    // Cell Timing [1], Cell Timing SFN[0]
        {UINT, "RSRP Rx[0]", 4},    // skip 10 bits, then 12 bits. (0.0625 * x - 180) dBm
        {UINT, "RSRP Rx[1]", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
        {UINT, "RSRP", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
        {UINT, "RSRQ Rx[0]", 4},    // skip 12 bits, then 10 bits, (0.0625 * x - 30) dB
        {UINT, "RSRQ Rx[1]", 4},    // 10 bits, (0.0625 * x) - 30 dB
        {PLACEHOLDER, "RSRQ", 0},   // skip 20 bits, then 10 bits, (0.0625 * x - 30) dB
        {UINT, "RSSI Rx[0]", 4},    // skip 10 bits, them 11 bits (0.0625 * x - 110) dBm
        {PLACEHOLDER, "RSSI Rx[1]", 0}, // skip 21 bits, then 11 bits (0.0625 * x - 110) dBm
        {UINT, "RSSI", 4},    // 11 bits, (0.0625 * x - 110) dBm
        {SKIP, NULL, 20},
        {UINT, "FTL SNR Rx[0]", 4}, // 9 bits
        {PLACEHOLDER, "FTL SNR Rx[1]", 0},  // skip 9 bits, then 9 bits (0.1 * x - 20) dB
        {SKIP, NULL, 12},
        {UINT, "Projected SIR", 4}, // Projected Sir, if x & 1<<31: x -= 4294967296
        // x /= 16
        {UINT, "Post IC RSRQ", 4},  // (0.0625 * x - 30) dB
};

const Fmt LtePhySubpktFmt_v1_Scmr_v22 [] = {
        {UINT, "E-ARFCN", 4},
        {UINT, "Num-of-cells", 2},
        {SKIP, NULL, 2},
        {UINT, "Physical Cell ID", 2},  // 9 bits
        {PLACEHOLDER, "Serving Cell Index", 0},    // 3 bits
        {PLACEHOLDER, "Is Serving Cell", 0},    // 1 bit
        {SKIP, NULL, 2},
        {UINT, "Current SFN", 2},   // 10 bits
        {PLACEHOLDER, "Current Subframe Number", 0},    // 4 bits
        {SKIP, NULL, 2},
        {SKIP, NULL, 4},    // Cell Timing [0]
        {SKIP, NULL, 4},    // Cell Timing [1], Cell Timing SFN [0]
        {UINT, "RSRP Rx[0]", 4},    // skip 10 bits, then 12 bits. (0.0625 * x - 180) dBm
        {UINT, "RSRP Rx[1]", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
        {SKIP, NULL, 4},
        {UINT, "RSRP", 4},    // skip 12 bits, then 12 bits (0.0625 * x - 180) dBm
        {UINT, "RSRQ Rx[0]", 4},    // skip 12 bits, then 10 bits, (0.0625 * x - 30) dB
        {UINT, "RSRQ Rx[1]", 4},    // 10 bits, (0.0625 * x) - 30 dB
        {UINT, "RSRQ", 4},    // skip 10 bits, then 10 bits, (0.0625 * x - 30) dB
        {UINT, "RSSI Rx[0]", 4},    // skip 10 bits, them 11 bits (0.0625 * x - 110) dBm
        {PLACEHOLDER, "RSSI Rx[1]", 0}, // skip 21 bits, then 11 bits (0.0625 * x - 110) dBm
        {SKIP, NULL, 4},
        {UINT, "RSSI", 4},    // then 11 bits, (0.0625 * x - 110) dBm
        {SKIP, NULL, 20},
        {UINT, "FTL SNR Rx[0]", 4}, // 9 bits
        {PLACEHOLDER, "FTL SNR Rx[1]", 0},  // skip 9 bits, then 9 bits (0.1 * x - 20) dB
        {SKIP, NULL, 16},
        {UINT, "Projected SIR", 4}, // Projected Sir, if x & 1<<31: x -= 4294967296
        // x /= 16
        {UINT, "Post IC RSRQ", 4},  // (0.0625 * x - 30) dB
        {UINT, "CINR Rx[0]", 4},
        {UINT, "CINR Rx[1]", 4},
};

const Fmt LtePhySubpktFmt [] = {
        {UINT, "Version", 1},
        {UINT, "Number of SubPackets", 1},
        {SKIP, NULL, 2}     // Unknown
};

const Fmt LtePhySubpktFmt_v1_SubpktHeader [] = {
        {UINT, "SubPacket ID", 1},
        {UINT, "SubPacket Version", 1},
        {UINT, "SubPacket Size", 2},
};

const ValueName LtePhySubpkt_SubpktType [] = {
        {25, "Serving_Cell_Measurement_Result"},
        {26, "Serving_Cell_Measurement_Result"},
        {27, "Serving_Cell_Measurement_Result"}
};

// Serving_Cell_Measurement_Result
const Fmt LtePhySubpktFmt_v1_Scmr_v4 [] = {
        {UINT, "E-ARFCN", 2},
        {UINT, "Physical Cell ID", 2}   //cell ID
};

// ------------------------------------------------------------
// NR_RRC_OTA_Packet
const Fmt NrRrcOtaPacketFmt[] = {
        {UINT, "Pkt Version",        1},    //version
        {UINT, "Unknown", 			 3},    //Maybe Reserved
        {UINT, "RRC Release Number", 1},    //RRC release version
        {UINT, "RRC Version Number", 1},    //RRC version version
        {UINT, "Radio Bearer ID",    1},    //no change
        {UINT, "Physical Cell ID",   2},     //Cell ID
        {UINT, "Freq",   			 4},     //Freq
        {UINT, "SysFrameNum/SubFrameNum",   4},     //System/subsystem frame number
        {UINT, "Physical Cell ID",   2},     //Cell ID
        {UINT, "PDU Number",         2},     //PDU Number
        {UINT, "SIB Mask In SI",     4},
        {UINT, "Msg Length",         2}
};

const ValueName NrRrcOtaPduType_v7[] = {
//        {0x02, "LTE-RRC_BCCH_DL_SCH"},
//        {0x04, "LTE-RRC_PCCH"},
//        {0x05, "LTE-RRC_DL_CCCH"},
//        {0x06, "LTE-RRC_DL_DCCH"},
//        {0x07, "LTE-RRC_UL_CCCH"},
//        {0x08, "LTE-RRC_UL_DCCH"},
};

const ValueName NrRrcOtaPduType_v8[] = {
//        {0x02, "LTE-RRC_BCCH_DL_SCH"},
//        {0x04, "LTE-RRC_PCCH"},
//        {0x05, "LTE-RRC_DL_CCCH"},
//        {0x06, "LTE-RRC_DL_DCCH"},
//        {0x07, "LTE-RRC_UL_CCCH"},
//        {0x08, "LTE-RRC_UL_DCCH"},
};

// ------------------------------------------------------------




static int
_decode_nr_rrc_ota(const char *b, int offset, size_t length,
                   json &j) {
    int start = offset;
//    int pkt_ver = _search_result_int(result, "Pkt Version");
//
//    //pkt_ver==8 (Xiaomi)
//    if (pkt_ver == 8) {
//
//        int pdu_number = _search_result_int(result, "PDU Number");
//        int pdu_length = _search_result_int(result, "Msg Length");
//
//        const char *type_name = search_name(NrRrcOtaPduType_v8,
//                                            ARRAY_SIZE(NrRrcOtaPduType_v8, ValueName),
//                                            pdu_number);
//
//        if (type_name == NULL) {    // not found
//            printf("(MI)Unknown 5G NR RRC PDU Type: 0x%x\n", pdu_number);
//            return 0;
//        } else {
//            std::string type_str = "raw_msg/";
//            type_str += type_name;
//            PyObject *t = Py_BuildValue("(sy#s)",
//                                        "Msg", b + offset, pdu_length, type_str.c_str());
//            PyList_Append(result, t);
//            Py_DECREF(t);
//            return (offset - start) + pdu_length;
//        }
//
//    } else if (pkt_ver = 7) {
//        //pkt_ver==8 (Samsung)
//        int pdu_number = _search_result_int(result, "PDU Number");
//        int pdu_length = _search_result_int(result, "Msg Length");
//        const char *type_name = search_name(NrRrcOtaPduType_v7,
//                                            ARRAY_SIZE(NrRrcOtaPduType_v7, ValueName),
//                                            pdu_number);
//
//        if (type_name == NULL) {    // not found
//            printf("(MI)Unknown 5G NR RRC PDU Type: 0x%x\n", pdu_number);
//            return 0;
//        } else {
//            std::string type_str = "raw_msg/";
//            type_str += type_name;
//            PyObject *t = Py_BuildValue("(sy#s)",
//                                        "Msg", b + offset, pdu_length, type_str.c_str());
//            PyList_Append(result, t);
//            Py_DECREF(t);
//            return (offset - start) + pdu_length;
//        }
//    }
}

// ----------------------------------------------------------------------------


static int
_decode_lte_phy_subpkt(const char *b, int offset, size_t length,
                       json &j) {
    int start = offset;
    int pkt_ver = j["payload"]["LtePhySubpkt"]["header"]["Version"];
    int n_subpkt = j["payload"]["LtePhySubpkt"]["header"]["Number of SubPackets"];

    j["payload"]["LtePhySubpkt"]["subpackets"] = {};

    switch (pkt_ver) {
        case 1:
        {
            //PyObject *result_allpkts = PyList_New(0);

            for (int i = 0; i < n_subpkt; i++) {
                //PyObject *result_subpkt = PyList_New(0);
                json jsp; // Json For Subpacket
                // Decode subpacket header
                offset += _decode_by_fmt(LtePhySubpktFmt_v1_SubpktHeader,
                                         ARRAY_SIZE(LtePhySubpktFmt_v1_SubpktHeader, Fmt),
                                         b, offset, length, jsp);
                // Decode payload
                int subpkt_id = jsp["SubPacket ID"];
                int subpkt_ver = jsp["SubPacket Version"];

                const char *type_name = search_name(LtePhySubpkt_SubpktType,
                                                    ARRAY_SIZE(LtePhySubpkt_SubpktType, ValueName),subpkt_id);
                if (type_name == NULL) {    // not found
                    LOGD("(MI)Unknown LTE PHY Subpacket ID: 0x%x\n", subpkt_id);
                } else {
                    bool success = false;
                    if (strcmp(type_name, "Serving_Cell_Measurement_Result") == 0) {
                        switch (subpkt_ver) {
                            case 4:
                            {
                                offset += _decode_by_fmt(
                                        LtePhySubpktFmt_v1_Scmr_v4,
                                        ARRAY_SIZE(LtePhySubpktFmt_v1_Scmr_v4,
                                                   Fmt),
                                        b, offset, length, jsp);
                                success = true;
                                break;
                            }
                            case 19:
                            {
                                offset += _decode_by_fmt(
                                        LtePhySubpktFmt_v1_Scmr_v19,
                                        ARRAY_SIZE(LtePhySubpktFmt_v1_Scmr_v19,
                                                   Fmt),
                                        b, offset, length, jsp);
                                int temp = jsp["Physical Cell ID"];
                                int iPhyCellId = temp & 511;   // 9 bits
                                int iServingCellIdx = (temp >> 9) & 7; // 3 bits
                                int iIsServingCell = (temp >> 12) & 1;  // 1 bit

                                jsp["Physical Cell ID"] = iPhyCellId;
                                jsp["Serving Cell Index"] = iServingCellIdx;

                                const char *tmp_name = search_name(ValueNameCellIndex,
                                                                   ARRAY_SIZE(ValueNameCellIndex, ValueName), jsp["Serving Cell Index"]);

                                jsp["Serving Cell Index"]=((tmp_name == NULL)?"Unknown":tmp_name);
                                jsp["Is Serving Cell"] = iIsServingCell;



                                temp = jsp["Current SFN"];

                                int iSysFN = temp & 1023;   // 10 bits
                                int iSubFN = (temp >> 10) & 15; // 4 bits

                                jsp["Current SFN"] = iSysFN;
                                jsp["Current Subframe Number"] = iSubFN;

                                unsigned int utemp;

                                utemp = jsp["RSRP Rx[0]"];
                                float RSRP0 = float((utemp >> 10) & 4095);
                                RSRP0 = RSRP0 * 0.0625 - 180.0;
                                jsp["RSRP Rx[0]"] = RSRP0;

                                utemp = jsp["RSRP Rx[1]"];
                                float RSRP1 = float((utemp >> 12) & 4095);
                                RSRP1 = RSRP1 * 0.0625 - 180.0;
                                jsp["RSRP Rx[1]"] = RSRP1;

                                utemp = jsp["RSRP"];
                                float RSRP = float((utemp >> 12) & 4095);
                                RSRP = RSRP * 0.0625 - 180.0;
                                jsp["RSRP"] = RSRP;

                                utemp = jsp["RSRQ Rx[0]"];
                                float RSRQ0 = float((utemp >> 12) & 1023);
                                RSRQ0 = RSRQ0 * 0.0625 - 30.0;
                                jsp["RSRQ Rx[0]"] = RSRQ0;

                                utemp = jsp["RSRQ Rx[1]"];
                                float RSRQ1 = float(utemp & 1023);
                                RSRQ1 = RSRQ1 * 0.0625 - 30.0;
                                jsp["RSRQ Rx[1]"] = RSRQ1;

                                float RSRQ = float((utemp >> 20) & 1023);
                                RSRQ = RSRQ * 0.0625 - 30.0;
                                jsp["RSRQ"] = RSRQ;

                                utemp = jsp["RSSI Rx[0]"];
                                float RSSI0 = float((utemp >> 10) & 2047);
                                RSSI0 = RSSI0 * 0.0625 - 110.0;
                                jsp["RSSI Rx[0]"] = RSSI0;

                                float RSSI1 = float((utemp >> 21) & 2047);
                                RSSI1 = RSSI1 * 0.0625 - 110.0;
                                jsp["RSSI Rx[1]"] = RSSI1;

                                utemp = jsp["RSSI"];
                                float RSSI = float(utemp & 2047);
                                RSSI = RSSI * 0.0625 - 110.0;
                                jsp["RSSI"] = RSSI;

                                utemp = jsp["FTL SNR Rx[0]"];
                                float SNR0 = float(utemp & 511);
                                SNR0 = SNR0 * 0.1 - 20.0;
                                jsp["FTL SNR Rx[0]"] = SNR0;

                                float SNR1 = float((utemp >> 9) & 511);
                                SNR1 = SNR1 * 0.1 - 20.0;
                                jsp["FTL SNR Rx[1]"] = SNR1;

                                utemp = jsp["Post IC RSRQ"];
                                float postICRSRQ = float(utemp * 0.0625 - 30);
                                jsp["Post IC RSRQ"] = postICRSRQ;

                                utemp = jsp["Projected SIR"];
                                int SIR = 0;
                                if (utemp & (1 << 31)) {
                                    SIR = utemp - 4294967296;
                                } else {
                                    SIR = utemp;
                                }
                                SIR = SIR / 16;
                                jsp["Projected SIR"] = SIR;

                                success = true;
                                break;
                            }
                            case 22:
                            {
                                offset += _decode_by_fmt(
                                        LtePhySubpktFmt_v1_Scmr_v22,
                                        ARRAY_SIZE(LtePhySubpktFmt_v1_Scmr_v22,
                                                   Fmt),
                                        b, offset, length, jsp);
                                int temp = jsp["Physical Cell ID"];
                                int iPhyCellId = temp & 511;   // 9 bits
                                int iServingCellIdx = (temp >> 9) & 7; // 3 bits
                                int iIsServingCell = (temp >> 12) & 1;  // 1 bit
                                jsp["Physical Cell ID"] = iPhyCellId;
                                jsp["Serving Cell Index"] = iServingCellIdx;

                                const char *tmp_name = search_name(ValueNameCellIndex,
                                                                   ARRAY_SIZE(ValueNameCellIndex, ValueName), jsp["Serving Cell Index"]);

                                jsp["Serving Cell Index"]=((tmp_name == NULL)?"Unknown":tmp_name);
                                jsp["Is Serving Cell"] = iIsServingCell;

                                temp = jsp["Current SFN"];
                                int iSysFN = temp & 1023;   // 10 bits
                                int iSubFN = (temp >> 10) & 15; // 4 bits
                                jsp["Current SFN"] = iSysFN;
                                jsp["Current Subframe Number"] = iSubFN;

                                unsigned int utemp;

                                utemp = jsp["RSRP Rx[0]"];
                                float RSRP0 = float((utemp >> 10) & 4095);
                                RSRP0 = RSRP0 * 0.0625 - 180.0;
                                jsp["RSRP Rx[0]"] = RSRP0;


                                utemp = jsp["RSRP Rx[1]"];
                                float RSRP1 = float((utemp >> 12) & 4095);
                                RSRP1 = RSRP1 * 0.0625 - 180.0;
                                jsp["RSRP Rx[1]"] = RSRP1;

                                utemp = jsp["RSRP"];
                                float RSRP = float((utemp >> 12) & 4095);
                                RSRP = RSRP * 0.0625 - 180.0;
                                jsp["RSRP"] = RSRP;

                                utemp = jsp["RSRQ Rx[0]"];
                                float RSRQ0 = float((utemp >> 12) & 1023);
                                RSRQ0 = RSRQ0 * 0.0625 - 30.0;
                                jsp["RSRQ Rx[0]"] = RSRQ0;

                                utemp = jsp["RSRQ Rx[1]"];
                                float RSRQ1 = float(utemp & 1023);
                                RSRQ1 = RSRQ1 * 0.0625 - 30.0;
                                jsp["RSRQ Rx[1]"] = RSRQ1;

                                float RSRQ = float((utemp >> 10) & 1023);
                                RSRQ = RSRQ * 0.0625 - 30.0;
                                jsp["RSRQ"] = RSRQ;

                                utemp = jsp["RSSI Rx[0]"];
                                float RSSI0 = float((utemp >> 10) & 2047);
                                RSSI0 = RSSI0 * 0.0625 - 110.0;
                                jsp["RSSI Rx[0]"] = RSSI0;

                                float RSSI1 = float((utemp >> 21) & 2047);
                                RSSI1 = RSSI1 * 0.0625 - 110.0;
                                jsp["RSSI Rx[1]"] = RSSI1;

                                utemp = jsp["RSSI"];
                                float RSSI = float(utemp & 2047);
                                RSSI = RSSI * 0.0625 - 110.0;
                                jsp["RSSI"] = RSSI;

                                utemp = jsp["FTL SNR Rx[0]"];
                                float SNR0 = float(utemp & 511);
                                SNR0 = SNR0 * 0.1 - 20.0;
                                jsp["FTL SNR Rx[0]"] = SNR0;

                                float SNR1 = float((utemp >> 9) & 511);
                                SNR1 = SNR1 * 0.1 - 20.0;
                                jsp["FTL SNR Rx[1]"] = SNR1;

                                utemp = jsp["Post IC RSRQ"];
                                float postICRSRQ = float(utemp * 0.0625 - 30);
                                jsp["Post IC RSRQ"] = postICRSRQ;

                                utemp = jsp["Projected SIR"];
                                int SIR = 0;
                                if (utemp & (1 << 31)) {
                                    SIR = utemp - 4294967296;
                                } else {
                                    SIR = utemp;
                                }
                                SIR = SIR / 16;
                                jsp["Projected SIR"] = SIR;

                                success = true;
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    // TODO: replace type ID to name.

                    if (success) {
                        j["payload"]["LtePhySubpkt"]["subpackets"].push_back(jsp);
                    } else {
                        LOGD("Unknown LTE PHY Subpacket version: 0x%x - %d\n", subpkt_id, subpkt_ver);
                    }
                }
            }
            return offset - start;
        }
        default:
            LOGD("Unknown LTE PHY packet version: 0x%x\n", pkt_ver);
            return 0;
    }
}

#endif //WITNETTEST004_LTE_PHY_SERVING_CELL_MEAS_RES_H
