//
// Created by siralex on 20-Apr-18.
//

#ifndef WITNETTEST004_GSM_RR_CELL_RESELECTION_MEAS_H
#define WITNETTEST004_GSM_RR_CELL_RESELECTION_MEAS_H
#include "log_packet_utils.h"
#include "utils.h"
#include "consts.h"
#include <android/log.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

const Fmt GsmRrCellResMeas_Fmt [] = {
        {UINT, "Serving BCCH ARFCN", 2},    // 12 bits
        {PLACEHOLDER, "Serving BCCH Band", 0},  // 4 bits
        {UINT, "Serving PBCCH ARFCN", 2},   // 12 bits
        {PLACEHOLDER, "Serving PBCCH Band", 0}, // 4 bits
        {UINT, "Serving Priority Class", 1},
        {UINT, "Serving RX Level Average", 1},  // x - 110.5
        {UINT, "Serving C1 Value", 4},
        {UINT, "Serving C2 Value", 4},
        {UINT, "Serving C31 Value", 4},
        {UINT, "Serving C32 Value", 4},
        {UINT, "Serving 5 Sec Timer State", 1},
        {UINT, "Serving Barred for Reselection", 1},
        {UINT, "Serving Cell Reselection in Last 15 Seconds", 1},
        {UINT, "Neighboring 6 Strongest Cells Count", 1},
};

const Fmt GsmRrCellResMeas_NeighborCell [] = {
        {UINT, "BCCH ARFCN", 2},    // 12 bits
        {PLACEHOLDER, "BCCH Band", 0},  // 4 bits
        {UINT, "PBCCH ARFCN", 2},   // 12 bits
        {PLACEHOLDER, "PBCCH Band", 0}, // 4 bits
        {UINT, "Priority Class", 1},
        {UINT, "RX Level Average", 1},  // x - 110.5
        {UINT, "C1 Value", 4},
        {UINT, "C2 Value", 4},
        {UINT, "C31 Value", 4},
        {UINT, "C32 Value", 4},
        {UINT, "5 Sec Timer State", 1},
        {UINT, "Barred for Reselection", 1},
        {UINT, "Belong to Serving Cell's RA", 1},
};


static int _decode_gsm_rcrm_payload (const char *b,
                                     int offset, size_t length, json &j) {
    int start = offset;
    int temp;
    char* tmp_name;

    temp = j["payload"]["GsmRrCellResMeas"]["Serving BCCH ARFCN"];
    int iServingBCCH = temp & 4095; // 12 bits
    int iServingBCCHBand = (temp >> 12) & 15;
    j["payload"]["GsmRrCellResMeas"]["Serving BCCH ARFCN"] = iServingBCCH;

    j["payload"]["GsmRrCellResMeas"]["Serving BCCH Band"] = iServingBCCHBand;

    tmp_name = (char *)search_name(ValueNameBandClassGSM,
                                   ARRAY_SIZE(ValueNameBandClassGSM, ValueName),
                                   j["payload"]["GsmRrCellResMeas"]["Serving BCCH Band"]);

    j["payload"]["GsmRrCellResMeas"]["Serving BCCH Band"] = ((tmp_name == NULL)?"Unknown":tmp_name);

    temp = j["payload"]["GsmRrCellResMeas"]["Serving PBCCH ARFCN"];
    int iServingPBCCH = temp & 4095; // 12 bits
    int iServingPBCCHBand = (temp >> 12) & 15;
    j["payload"]["GsmRrCellResMeas"]["Serving PBCCH ARFCN"] = iServingPBCCH;
    j["payload"]["GsmRrCellResMeas"]["Serving PBCCH Band"] = iServingPBCCHBand;

    tmp_name = (char *) search_name(ValueNameBandClassGSM,
                                    ARRAY_SIZE(ValueNameBandClassGSM, ValueName),
                                    j["payload"]["GsmRrCellResMeas"]["Serving PBCCH Band"]);
    j["payload"]["GsmRrCellResMeas"]["Serving PBCCH Band"] = ((tmp_name == NULL)?"Unknown":tmp_name);

    temp = j["payload"]["GsmRrCellResMeas"]["Serving RX Level Average"];
    float fRx = temp - 110.5;

    j["payload"]["GsmRrCellResMeas"]["Serving RX Level Average"] = fRx;

    tmp_name = (char *) search_name(ValueNameTimerState,
                                    ARRAY_SIZE(ValueNameTimerState, ValueName),
                                    j["payload"]["GsmRrCellResMeas"]["Serving 5 Sec Timer State"]);
    j["payload"]["GsmRrCellResMeas"]["Serving 5 Sec Timer State"] = ((tmp_name == NULL)?"Unknown":tmp_name);


    tmp_name = (char *) search_name(ValueNameYesOrNo,
                                    ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                                    j["payload"]["GsmRrCellResMeas"]["Serving Barred for Reselection"]);
    j["payload"]["GsmRrCellResMeas"]["Serving Barred for Reselection"] = ((tmp_name == NULL)?"Unknown":tmp_name);

    tmp_name = (char *) search_name(ValueNameYesOrNo,
                                    ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                                    j["payload"]["GsmRrCellResMeas"]["Serving Cell Reselection in Last 15 Seconds"]);
    j["payload"]["GsmRrCellResMeas"]["Serving Cell Reselection in Last 15 Seconds"] = ((tmp_name == NULL)?"Unknown":tmp_name);

    int num_record = j["payload"]["GsmRrCellResMeas"]["Neighboring 6 Strongest Cells Count"];

    json jca = {}; // json cell array
    for (int i = 0; i < num_record; i++) {
        json jci; // json cell info
        offset += _decode_by_fmt(GsmRrCellResMeas_NeighborCell,
                                 ARRAY_SIZE(GsmRrCellResMeas_NeighborCell, Fmt),
                                 b, offset, length, jci);

        temp = jci["BCCH ARFCN"];
        int iBCCH = temp & 4095; // 12 bits
        int iBCCHBand = (temp >> 12) & 15;
        jci["BCCH ARFCN"] = iBCCH;

        jci["BCCH Band"] = iBCCHBand;

        tmp_name = (char *) search_name(ValueNameBandClassGSM,
                                        ARRAY_SIZE(ValueNameBandClassGSM, ValueName),
                                        jci["BCCH Band"]);
        jci["BCCH Band"] = ((tmp_name == NULL)?"Unknown":tmp_name);


        temp = jci["PBCCH ARFCN"];
        int iPBCCH = temp & 4095; // 12 bits
        int iPBCCHBand = (temp >> 12) & 15;
        jci["PBCCH ARFCN"] = iPBCCH;
        jci["PBCCH Band"] = iPBCCHBand;

        tmp_name = (char *) search_name(ValueNameBandClassGSM,
                                        ARRAY_SIZE(ValueNameBandClassGSM, ValueName),
                                        jci["PBCCH Band"]);
        jci["PBCCH Band"] = ((tmp_name == NULL)?"Unknown":tmp_name);

        temp = jci["RX Level Average"];
        float fRx = temp - 110.5;
        jci["RX Level Average"] = fRx;


        tmp_name = (char *) search_name(ValueNameTimerState,
                                        ARRAY_SIZE(ValueNameTimerState, ValueName),
                                        jci["5 Sec Timer State"]);
        jci["5 Sec Timer State"] = ((tmp_name == NULL)?"Unknown":tmp_name);

        tmp_name = (char *) search_name(ValueNameYesOrNo,
                                        ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                                        jci["Barred for Reselection"]);
        jci["Barred for Reselection"] = ((tmp_name == NULL)?"Unknown":tmp_name);


        tmp_name = (char *) search_name(ValueNameYesOrNo,
                                        ARRAY_SIZE(ValueNameYesOrNo, ValueName),
                                        jci["Belong to Serving Cell's RA"]);
        jci["Belong to Serving Cell's RA"] = ((tmp_name == NULL)?"Unknown":tmp_name);


        jca.push_back(jci);
    }

    j["payload"]["GsmRrCellResMeas"]["Neighboring 6 Strongest Cells"] = jca;

    return offset - start;
}




#endif //WITNETTEST004_GSM_RR_CELL_RESELECTION_MEAS_H
