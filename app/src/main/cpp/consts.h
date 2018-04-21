//
// Created by siralex on 27-Mar-18.
//

#ifndef WITNETTEST004_CONSTS_H
#define WITNETTEST004_CONSTS_H

#include <map>
#include <vector>
#include <array>

using namespace std;


// Define the required data types
typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;
typedef unsigned long long UINT64;

static const int ESCAPE_XOR = 0x20;

typedef pair<char*, int> BinaryBuffer;
typedef vector<int> IdVector;

struct ValueName {
    int val;
    const char *name;
};

struct PktValueName {
    int val;
    const char *name;
    bool enabled;
};

// Operations of log config messages
enum LogConfigOp {
    DISABLE 			= 0,
    GET_RANGE 			= 1,
    SET_MASK 			= 3,
    GET_MASK 			= 4,
    DEBUG_LTE_ML1 		= 105, 		// enable debugging messages (currently for iCellular only)
    DEBUG_WCDMA_L1 		= 106, 		// enable debugging messages (currently for iCellular only)
    DISABLE_DEBUG 		= 107,		// Disable all dbg msg
    DIAG_BEGIN 			= 108,		// Beginning of DIAG.CFG
    DIAG_END 			= 109,		// End of DIAG.CFG
    DIAG_BEGIN_1D 		= 110,
    DIAG_BEGIN_00 		= 111,
    DIAG_BEGIN_7C 		= 112,
    DIAG_BEGIN_1C	 	= 113,
    DIAG_BEGIN_0C 		= 114,
    DIAG_BEGIN_63 		= 115,
    DIAG_BEGIN_4B0F0000 = 116,
    DIAG_BEGIN_4B090000 = 117,
    DIAG_BEGIN_4B080000 = 118,
    DIAG_BEGIN_4B080100 = 119,
    DIAG_BEGIN_4B040000 = 120,
    DIAG_BEGIN_4B040F00 = 121,
    DIAG_END_6000 		= 122,
};

// All supported type IDs
enum LogPacketType {
    CDMA_Paging_Channel_Message 							= 0x1007,

    _1xEV_Connection_Attempt 								= 0x106E,
    _1xEV_Connection_Release 								= 0x1071,
    // _1xEV_Signaling_Control_Channel_Broadcast 			= 0x107C,
    _1xEV_Connected_State_Search_Info 						= 0x118A,
    Srch_TNG_1x_Searcher_Dump 								= 0x119B,
    _1xEV_Rx_Partial_MultiRLP_Packet 						= 0x1277,
    _1xEVDO_Multi_Carrier_Pilot_Sets 						= 0x12A1,
    HDR_Pilot_Sets 											= 0x12B6,    // TODO

    WCDMA_Search_Cell_Reselection_Rank 						= 0x4005,
    WCDMA_RRC_States 										= 0x4125,
    WCDMA_CELL_ID 											= 0x4127,
    WCDMA_Signaling_Messages 								= 0x412F,

    UMTS_NAS_GMM_State 										= 0x7130,
    UMTS_NAS_MM_State 										= 0x7131,
    UMTS_NAS_MM_REG_State 									= 0x7135,
    UMTS_NAS_OTA 											= 0x713A,

    LTE_RRC_OTA_Packet 										= 0xB0C0,
    LTE_RRC_MIB_Message_Log_Packet	 						= 0xB0C1,
    LTE_RRC_Serv_Cell_Info_Log_Packet 						= 0xB0C2,

    LTE_NAS_ESM_Plain_OTA_Incoming_Message 					= 0xB0E2,
    LTE_NAS_ESM_Plain_OTA_Outgoing_Message 					= 0xB0E3,
    LTE_NAS_EMM_Plain_OTA_Incoming_Message 					= 0xB0EC,
    LTE_NAS_EMM_Plain_OTA_Outgoing_Message 					= 0xB0ED,
    LTE_NAS_EMM_State 										= 0xB0EE,
    LTE_NAS_ESM_State 										= 0xB0E5,

    LTE_PDCP_DL_Config 										= 0xB0A0,
    LTE_PDCP_UL_Config 										= 0xB0B0,
    LTE_PDCP_UL_Data_PDU 									= 0xB0B1,
    LTE_PDCP_DL_Ctrl_PDU 									= 0xB0A2,
    LTE_PDCP_UL_Ctrl_PDU 									= 0xB0B2,
    LTE_PDCP_DL_Cipher_Data_PDU 							= 0xB0A3,
    LTE_PDCP_UL_Cipher_Data_PDU 							= 0xB0B3,
    LTE_PDCP_DL_Stats 										= 0xB0A4,
    LTE_PDCP_UL_Stats 										= 0xB0B4,
    LTE_PDCP_DL_SRB_Integrity_Data_PDU 						= 0xB0A5,
    LTE_PDCP_UL_SRB_Integrity_Data_PDU 						= 0xB0B5,

    LTE_MAC_Configuration									= 0xB060,
    LTE_MAC_Rach_Trigger 									= 0xB061,
    LTE_MAC_Rach_Attempt 									= 0xB062,
    LTE_MAC_DL_Transport_Block 								= 0xB063,
    LTE_MAC_UL_Transport_Block 								= 0xB064,
    LTE_MAC_UL_Buffer_Status_Internal 						= 0xB066,
    LTE_MAC_UL_Tx_Statistics 								= 0xB067,

    LTE_RLC_UL_Config_Log_Packet 							= 0xB091,
    LTE_RLC_DL_Config_Log_Packet 							= 0xB081,
    LTE_RLC_UL_AM_All_PDU 									= 0xB092,
    LTE_RLC_DL_AM_All_PDU 									= 0xB082,
    LTE_RLC_UL_Stats 										= 0xB097,
    LTE_RLC_DL_Stats 										= 0xB087,

    LTE_PUCCH_Power_Control 								= 0xB16F,
    LTE_PUSCH_Power_Control 								= 0xB16E,
    LTE_PDCCH_PHICH_Indication_Report 						= 0xB16B,
    LTE_PDSCH_Stat_Indication 								= 0xB173,

    LTE_PHY_Connected_Mode_LTE_Intra_Freq_Meas_Results 		= 0xB179,
    LTE_PHY_CDMA_MEAS	 									= 0xB185,
    LTE_PHY_IRAT_Measurement_Request 						= 0xB187,
    LTE_PHY_IRAT_MDB 										= 0xB188,
    LTE_PHY_RLM_Report 										= 0xB18A,
    LTE_PHY_System_Scan_Results 							= 0xB18E,
    LTE_PHY_Idle_Neighbor_Cell_Meas 						= 0xB192,
    LTE_PHY_Serving_Cell_Measurement_Result 				= 0xB193,
    LTE_PHY_Connected_Mode_Neighbor_Meas_Req_Resp 			= 0xB195,
    LTE_PHY_CDRX_Events_Info 								= 0xB198,
    LTE_PHY_BPLMN_Cell_Request 								= 0xB1A1,
    LTE_PHY_BPLMN_Cell_Confirm 								= 0xB1A2,

    LTE_PHY_Serving_Cell_COM_Loop 							= 0xB121,
    LTE_PHY_PDSCH_Demapper_Configuration 					= 0xB126,
    LTE_PHY_PDCCH_Decoding_Result 							= 0xB130,
    LTE_PHY_PDSCH_Decoding_Result 							= 0xB132,
    LTE_PHY_PUSCH_Tx_Report 								= 0xB139,
    LTE_PHY_PUCCH_Tx_Report 								= 0xB13C,
    LTE_PHY_PUCCH_CSF 										= 0xB14D,
    LTE_PHY_PUSCH_CSF 										= 0xB14E,

    GSM_Surround_Cell_BA_List 								= 0x5071,
    GSM_RR_Cell_Selection_Parameters 						= 0x5130,  // TODO
    GSM_RR_Cell_Information 								= 0x5134,
    GSM_RR_Cell_Reselection_Meas 							= 0x51FC,

    Modem_debug_message 									= 0x1FEB,
};

// TODO
// в будущем на основе этого массива надо сделать отображение простых имен в пользовательском интерфейсе в
// группы реальных айдишников сообщений.

const array<PktValueName, 79> LogPacketTypeID_To_Name = {{
    {CDMA_Paging_Channel_Message,                       "CDMA_Paging_Channel_Message", false},
    {_1xEV_Connection_Attempt,                          "1xEV_Connection_Attempt", false},
    {_1xEV_Connection_Release,                          "1xEV_Connection_Release", false},
    // Yuanjie: Buggy parsing!!! Disable _1xEV_Signaling_Control_Channel_Broadcast temporarily
    // {_1xEV_Signaling_Control_Channel_Broadcast,      "1xEV_Signaling_Control_Channel_Broadcast",false},
    {_1xEV_Connected_State_Search_Info,                 "1xEV_Connected_State_Search_Info", false},
    {Srch_TNG_1x_Searcher_Dump,                         "Srch_TNG_1x_Searcher_Dump", false},
    {_1xEV_Rx_Partial_MultiRLP_Packet,                  "1xEV_Rx_Partial_MultiRLP_Packet", false},
    {_1xEVDO_Multi_Carrier_Pilot_Sets,                  "_1xEVDO_Multi_Carrier_Pilot_Sets", false},
    {HDR_Pilot_Sets,                                    "HDR_Pilot_Sets", false},

    {WCDMA_Search_Cell_Reselection_Rank,                "WCDMA_Search_Cell_Reselection_Rank", false},
    {WCDMA_RRC_States,                                  "WCDMA_RRC_States", false},
    {WCDMA_CELL_ID,                                     "WCDMA_RRC_Serv_Cell_Info", true},
    {WCDMA_Signaling_Messages,                          "WCDMA_RRC_OTA_Packet", true},

    {UMTS_NAS_GMM_State,                                 "UMTS_NAS_GMM_State", false},
    {UMTS_NAS_MM_State,                                  "UMTS_NAS_MM_State", false},
    {UMTS_NAS_MM_REG_State,                              "UMTS_NAS_MM_REG_State", false},
    {UMTS_NAS_OTA,                                       "UMTS_NAS_OTA_Packet", false},

    {LTE_RRC_OTA_Packet,                                 "LTE_RRC_OTA_Packet", false},
    {LTE_RRC_MIB_Message_Log_Packet,                     "LTE_RRC_MIB_Packet", false},
    {LTE_RRC_MIB_Message_Log_Packet,                     "LTE_RRC_MIB_Message_Log_Packet", false},
    {LTE_RRC_Serv_Cell_Info_Log_Packet,                  "LTE_RRC_Serv_Cell_Info", false},

    {LTE_NAS_ESM_Plain_OTA_Incoming_Message,             "LTE_NAS_ESM_OTA_Incoming_Packet", false},
    {LTE_NAS_ESM_Plain_OTA_Outgoing_Message,             "LTE_NAS_ESM_OTA_Outgoing_Packet", false},
    {LTE_NAS_EMM_Plain_OTA_Incoming_Message,             "LTE_NAS_EMM_OTA_Incoming_Packet", false},
    {LTE_NAS_EMM_Plain_OTA_Outgoing_Message,             "LTE_NAS_EMM_OTA_Outgoing_Packet", false},
    {LTE_NAS_EMM_State,                                  "LTE_NAS_EMM_State", false},
    {LTE_NAS_ESM_State,                                  "LTE_NAS_ESM_State", false},

    {LTE_PDCP_DL_Config,                                 "LTE_PDCP_DL_Config", false},
    {LTE_PDCP_UL_Config,                                 "LTE_PDCP_UL_Config", false},
    {LTE_PDCP_UL_Data_PDU,                               "LTE_PDCP_UL_Data_PDU", false},
    {LTE_PDCP_DL_Ctrl_PDU,                               "LTE_PDCP_DL_Ctrl_PDU", false},
    {LTE_PDCP_UL_Ctrl_PDU,                               "LTE_PDCP_UL_Ctrl_PDU", false},
    {LTE_PDCP_DL_Cipher_Data_PDU,                        "LTE_PDCP_DL_Cipher_Data_PDU", false},
    {LTE_PDCP_UL_Cipher_Data_PDU,                        "LTE_PDCP_UL_Cipher_Data_PDU", false},
    {LTE_PDCP_DL_Stats,                                  "LTE_PDCP_DL_Stats", false},
    {LTE_PDCP_UL_Stats,                                  "LTE_PDCP_UL_Stats", false},
    {LTE_PDCP_DL_SRB_Integrity_Data_PDU,                 "LTE_PDCP_DL_SRB_Integrity_Data_PDU", false},
    {LTE_PDCP_UL_SRB_Integrity_Data_PDU,                 "LTE_PDCP_UL_SRB_Integrity_Data_PDU", false},

    {LTE_MAC_Configuration,                              "LTE_MAC_Configuration", false},
    {LTE_MAC_Rach_Trigger,                               "LTE_MAC_Rach_Trigger", false},
    {LTE_MAC_Rach_Attempt,                               "LTE_MAC_Rach_Attempt", false},
    {LTE_MAC_UL_Transport_Block,                         "LTE_MAC_UL_Transport_Block", false},
    {LTE_MAC_DL_Transport_Block,                         "LTE_MAC_DL_Transport_Block", false},
    {LTE_MAC_UL_Buffer_Status_Internal,                  "LTE_MAC_UL_Buffer_Status_Internal", false},
    {LTE_MAC_UL_Tx_Statistics,                           "LTE_MAC_UL_Tx_Statistics", false},

    {LTE_RLC_UL_Config_Log_Packet,                       "LTE_RLC_UL_Config_Log_Packet", false},
    {LTE_RLC_DL_Config_Log_Packet,                       "LTE_RLC_DL_Config_Log_Packet", false},
    {LTE_RLC_UL_AM_All_PDU,                              "LTE_RLC_UL_AM_All_PDU", false},
    {LTE_RLC_DL_AM_All_PDU,                              "LTE_RLC_DL_AM_All_PDU", false},
    {LTE_RLC_UL_Stats,                                   "LTE_RLC_UL_Stats", false},
    {LTE_RLC_DL_Stats,                                   "LTE_RLC_DL_Stats", false},
    {LTE_PUCCH_Power_Control,                            "LTE_PHY_PUCCH_Power_Control", false},
    {LTE_PUSCH_Power_Control,                            "LTE_PHY_PUSCH_Power_Control", false},
    {LTE_PDCCH_PHICH_Indication_Report,                  "LTE_PHY_PDCCH_PHICH_Indication_Report", false},
    {LTE_PDSCH_Stat_Indication,                          "LTE_PHY_PDSCH_Stat_Indication", false},

    {LTE_PHY_Connected_Mode_LTE_Intra_Freq_Meas_Results, "LTE_PHY_Connected_Mode_Intra_Freq_Meas", false},
    {LTE_PHY_CDMA_MEAS,                                  "LTE_PHY_Inter_RAT_CDMA_Measurement", false},
    {LTE_PHY_IRAT_Measurement_Request,                   "LTE_PHY_Inter_RAT_Measurement", false},
    {LTE_PHY_IRAT_MDB,                                   "LTE_PHY_Inter_RAT_Measurement", false},
    {LTE_PHY_RLM_Report,                                 "LTE_PHY_RLM_Report", false},
    {LTE_PHY_System_Scan_Results,                        "LTE_PHY_System_Scan_Results", false},
    {LTE_PHY_Idle_Neighbor_Cell_Meas,                    "LTE_PHY_Idle_Neighbor_Cell_Meas", false},
    {LTE_PHY_Serving_Cell_Measurement_Result,            "LTE_PHY_Serv_Cell_Measurement", true},
    {LTE_PHY_Connected_Mode_Neighbor_Meas_Req_Resp,      "LTE_PHY_Connected_Mode_Neighbor_Measurement", false},
    {LTE_PHY_CDRX_Events_Info,                           "LTE_RRC_CDRX_Events_Info", false},
    {LTE_PHY_BPLMN_Cell_Request,                         "LTE_PHY_BPLMN_Cell_Request", false},
    {LTE_PHY_BPLMN_Cell_Confirm,                         "LTE_PHY_BPLMN_Cell_Confirm", false},

    {LTE_PHY_Serving_Cell_COM_Loop,                      "LTE_PHY_Serving_Cell_COM_Loop", false},
    {LTE_PHY_PDSCH_Demapper_Configuration,               "LTE_PHY_PDSCH_Packet", false},
    {LTE_PHY_PDCCH_Decoding_Result,                      "LTE_PHY_PDCCH_Decoding_Result", false},
    {LTE_PHY_PDSCH_Decoding_Result,                      "LTE_PHY_PDSCH_Decoding_Result", false},
    {LTE_PHY_PUSCH_Tx_Report,                            "LTE_PHY_PUSCH_Tx_Report", false},
    {LTE_PHY_PUCCH_Tx_Report,                            "LTE_PHY_PUCCH_Tx_Report", false},
    {LTE_PHY_PUCCH_CSF,                                  "LTE_PHY_PUCCH_CSF", false},
    {LTE_PHY_PUSCH_CSF,                                  "LTE_PHY_PUSCH_CSF", false},

    {GSM_Surround_Cell_BA_List,                          "GSM_Surround_Cell_BA_List", false},
    {GSM_RR_Cell_Selection_Parameters,                   "GSM_RR_Cell_Selection_Parameters", false},
    {GSM_RR_Cell_Information,                            "GSM_RR_Cell_Information", false},
    {GSM_RR_Cell_Reselection_Meas,                       "GSM_RR_Cell_Reselection_Meas", true},

    {Modem_debug_message,                                 "Modem_debug_message", false}
 }};




#endif //WITNETTEST004_CONSTS_H
