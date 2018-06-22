//
// Created by siralex on 25-May-18.
//


#include <ws/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "packet-aww.h"
#include "utils.h"
#include <arpa/inet.h>
//#include <map>
#include <string>


extern "C" {
#include <ws/epan/epan.h>
#include <ws/epan/prefs.h>
#include <ws/epan/epan_dissect.h>
#include <ws/epan/tap.h>
#include <ws/epan/proto.h>
#include <ws/epan/packet_info.h>
#include <ws/epan/frame_data.h>
#include <ws/epan/packet.h>
#include <ws/epan/print.h>
#include <ws/wiretap/wtap.h>
#include <ws/wsutil/plugins.h>
#include <ws/epan/dissectors/packet-pdcp-lte.h>
#include <ws/register.h>
#include <ws/wsutil/privileges.h>
}


FILE* fmemopen(void* buf, size_t capacity, const char* mode);


std::map<std::string, unsigned int> KnownMessageTypes = {
// WCDMA: 100-199
// - WCDMA RRC: 100-149
        {"RRC_UL_CCCH",             100},
        {"RRC_UL_DCCH",             101},
        {"RRC_DL_CCCH",             102},
        {"RRC_DL_DCCH",             103},
        {"RRC_DL_BCCH_BCH",         104},
        {"RRC_DL_PCCH",             106},
// - WCDMA RRC SysInfo: 150-199
        {"RRC_MIB",                 150},
        {"RRC_SIB1",                151},
        {"RRC_SIB2",                152},
        {"RRC_SIB3",                153},
        {"RRC_SIB5",                155},
        {"RRC_SIB7",                157},
        {"RRC_SIB12",               162},
        {"RRC_SIB19",               169},
// - WCDMA RRC SchedulingBlock
        {"RRC_SB1",                 181},
// - NAS
        {"NAS",                     190},
// LTE: 200-299
        {"LTE-RRC_PCCH",            200},
        {"LTE-RRC_DL_DCCH",         201},
        {"LTE-RRC_UL_DCCH",         202},
        {"LTE-RRC_BCCH_DL_SCH",     203},
        {"LTE-RRC_DL_CCCH",         204},
        {"LTE-RRC_UL_CCCH",         205},
        {"LTE-NAS_EPS_PLAIN",       250},
        {"LTE-PDCP_DL_SRB",         300},
        {"LTE-PDCP_UL_SRB",         301}

};


const int BUFFER_SIZE = 2000;
guchar buffer[BUFFER_SIZE] = {};
static char xmlbuf[150000] = {};
unsigned char msgbuf[5000] = {};
epan_t *session;

void try_dissect(FILE *fp, size_t data_len, const guchar* raw_data)
{

    wtap_pkthdr phdr;
    frame_data fdata;

    memset(&phdr, 0, sizeof(wtap_pkthdr));

    frame_data_init(&fdata, 0, &phdr, 0, 0);

    // fdata = (frame_data*)g_new(frame_data, 1);

    // memset(fdata, 0, sizeof(frame_data));
    // fdata->pfd  = NULL;
    // fdata->num = 1;    // fdata->cum_bytes = 0;
    // fdata->file_off = 0;
    // fdata->subnum = 0;
    // fdata->interface_id = 0;
    fdata.pkt_len  = data_len;
    fdata.cap_len  = data_len;
    // fdata->cum_bytes = 0;
    // fdata->file_off = 0;
    // fdata->subnum = 0;
    // fdata.lnk_t = WTAP_ENCAP_ETHERNET;
    fdata.lnk_t = WTAP_ENCAP_USER1;
    fdata.flags.encoding = PACKET_CHAR_ENC_CHAR_ASCII;
    // fdata->flags.visited = 0;
    // fdata->flags.marked = 0;
    // fdata->flags.ref_time = 0;
    // fdata->color_filter = NULL;
    // fdata->abs_ts.secs = 0;
    // fdata->abs_ts.nsecs = 0;
    // fdata->opt_comment = NULL;

    epan_dissect_t *edt = epan_dissect_new(session, TRUE, TRUE);

    epan_dissect_run(edt, 0, &phdr, tvb_new_real_data(raw_data, data_len, data_len), &fdata, NULL);
    // const proto_tree *payload_tree = edt->tree->first_child->next;
    // print_tree(payload_tree, 0);

    write_pdml_proto_tree(edt, fp);

    epan_dissect_free(edt);

    frame_data_destroy(&fdata);

}



std::string process_message( unsigned char* indata, unsigned int datasize){

    std::string result;
    if(datasize < 2*sizeof(uint32_t)) return "packet too small";
    memcpy(buffer,indata,sizeof(uint32_t)*2);

//size_t ign = fread(buffer, sizeof(uint32_t), 2, stdin);
 //   if (ign < 2)
 //       break;
    unsigned int type = ntohl(*(uint32_t *)buffer);
    size_t data_len = ntohl(*((uint32_t *)(buffer + 4)));
    size_t offset = 8;
    size_t framingHeader_len = 0;

    if (type == 300 || type == 301) {
        /* If type is pdcp-lte signaling message, we need to add framing
         * header before read pdcp PDU. */
        /* Fixed start to each frame (allowing heuristic dissector to work
         * ) */
        memcpy(buffer + offset, PDCP_LTE_START_STRING,
               strlen(PDCP_LTE_START_STRING));
        offset += strlen(PDCP_LTE_START_STRING);

        /* Now write out fixed fields (the mandatory elements of struct
         * pdcp_lte_info */

        /* gboolean no_header_pdu */
        buffer[offset++] = FALSE;

        /* enum pdcp_plane */
        buffer[offset++] = SIGNALING_PLANE;

        /* gboolean rohc_compression */
        buffer[offset++] = FALSE;

        /* Optional fields */
        /* Direction */
        buffer[offset++] = PDCP_LTE_DIRECTION_TAG;
        switch (type) {
            case 300:   // downlink
                buffer[offset++] = DIRECTION_DOWNLINK;
                break;
            case 301:   // uplink
                buffer[offset++] = DIRECTION_UPLINK;
                break;
        }

        /* Logical Channel Type */
        buffer[offset++] = PDCP_LTE_LOG_CHAN_TYPE_TAG;
        buffer[offset++] = Channel_DCCH;

        /* BCCH Transport Type */
        buffer[offset++] = PDCP_LTE_BCCH_TRANSPORT_TYPE_TAG;
        buffer[offset++] = 0;

        buffer[offset++] = PDCP_LTE_PAYLOAD_TAG;
        framingHeader_len = offset - 8;
    }

    memcpy(buffer + offset, indata+(sizeof(uint32_t)*2), data_len);

    FILE *fp;
    memset(xmlbuf, 0, 150000);
    fp = fmemopen(xmlbuf, 150000, "w+");

    try_dissect( fp, data_len + 2 * 4 + framingHeader_len, buffer);
    fclose(fp);

    result.append(xmlbuf);

    return result;
}


void initialize_epan(void) {
    // to prevent "started_with_special_privs: assertion failed" error.
    init_process_policies();
    epan_init(register_all_protocols, register_all_protocol_handoffs, NULL, NULL );
    // !!
    prefs_register_modules();

    proto_register_aww();
    proto_reg_handoff_aww();


    //"uat:user_dlts:\"User 15 (DLT=162)\",\"payload protocol\",\"header size\",\"header protocol\",\"trailer size\",\"trailer protocol\""
    session = epan_new();
    char s[] = "uat:user_dlts:\"User 1 (DLT=148)\",\"aww\",\"0\",\"\",\"0\",\"\"";
    switch (prefs_set_pref(s)) {
        case PREFS_SET_OK:
            break;

        case PREFS_SET_SYNTAX_ERR:
        case PREFS_SET_NO_SUCH_PREF:
        case PREFS_SET_OBSOLETE:
        default:
            LOGD("Failed to set user_dlts.\n");
            return;
            break;
    }
    prefs_apply_all();


}

void stop_epan(void){

    epan_free(session);
    epan_cleanup();
}

unsigned char getcha(char h){
    switch(h){

        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'A':
        case 'a':
            return 10;
        case 'B':
        case 'b':
            return 11;
        case 'C':
        case 'c':
            return 12;
        case 'D':
        case 'd':
            return 13;
        case 'E':
        case 'e':
            return 14;
        case 'F':
        case 'f':
            return 15;
        default:
            return 100;
    }
}

std::string decode_msg(std::string msgType, std::string msgData){
    std::map<std::string, unsigned int>::iterator itr;

    itr = KnownMessageTypes.find(msgType);
    if (itr == KnownMessageTypes.end()) return "Unknown Message Type";
    if(msgData.size() % 2 != 0) return "Message must have even number of chars";
    unsigned int mt = htonl(itr->second);
    unsigned int msg_sz = 0;
    unsigned int offset = 8;

    for(int i=0; i<msgData.size(); i+=2){
        unsigned char h1 = getcha(msgData.at(i));
        unsigned char h2 = getcha(msgData.at(i+1));
        if((h1 > 16) || (h2>16)) return "character unrecognized";
        unsigned char h = (h1<<4) | h2;
        msgbuf[ offset + msg_sz ] = h;
        msg_sz++;
        //LOGD("%c %c %i %i %i %02X\n",msgData.at(i), msgData.at(i+1), h1, h2, h, h );
    }

    unsigned int msg_sz2 = htonl(msg_sz);
    memcpy(msgbuf+0, &mt, sizeof(int32_t));
    memcpy(msgbuf+4, &msg_sz2, sizeof(int32_t));

    std::string res = process_message( msgbuf, msg_sz+8);

    return res;


/*
  const char * innerdata = res.c_str();
    unsigned int strsz = res.size();
    unsigned char locbuf[513] = {};


    int pos = 0;
    for(int idx=0; idx<strsz; idx+=512) {
        memset(locbuf, 0, 513);
        if(idx+512<=strsz){
            pos = idx+512;
            memcpy(locbuf, innerdata+idx, 512);
            LOGD("%s", locbuf);
        }
    }

    if(pos<strsz){
        memset(locbuf, 0, 513);
        memcpy(locbuf, innerdata+pos, strsz-pos);
        LOGD("%s", locbuf);
    }

    return "OK";
*/
}

