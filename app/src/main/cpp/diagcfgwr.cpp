/*
 * diagcfgwr.cpp
 *
 *  Created on: Mar 7, 2018
 *      Author: siralex
 */
#include <map>
#include <vector>
#include <stdio.h>

#include "diagcfgwr.h"
#include "log_config.h"
#include "consts.h"
#include "utils.h"
#include "hdlc.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>


using namespace std;


static bool send_msg (FILE * DiagCfgFile, const char *b, int length);


// Converts vector of type names to a vector of IDs.
// Returns true if all string are successfully converted, or false if wrong name
// is found.
static bool
map_typenames_to_ids (vector<string> type_names, IdVector& type_ids) {

	for( string curTypeName:type_names) {
		int cnt = find_ids( curTypeName.c_str(), type_ids);
		if (cnt == 0) {return false;}
	}
	return true;
}

// sort, unique and bucketing
static void
sort_type_ids(IdVector& type_ids, std::vector<IdVector>& out_vectors) {
    sort(type_ids.begin(), type_ids.end()); // std::sort asc order
    IdVector::iterator itr = std::unique(type_ids.begin(), type_ids.end()); // удаляет одинаковые элементы и возвращает итератор установленный на конец нового диапазона
    type_ids.resize(std::distance(type_ids.begin(), itr)); // free memory of removed elements

/*
 дальше проходятся по отсортированному и очищенному от повторений вектору айдишников типов сообщений, которые пользователь хочет видеть
 и вместо вектора из интов, коими являются данные айдишники, делают вектор из векторов.
 во вложенные вектора айдишники группируются таким образом, чтобы в каждом вложенном векторе были айдишники характеризующиеся одинаковым equip_id
 equip_id - это цифра определяющая четвертую тетраду (полубайт) в айдишнике запрашиваемого сообщения
 например если запрошено логирование сообщений LTE_PDCP_UL_Stats, то согласно LogPacketType его айдишник 0xB0B4 и соответственно equip_id = 0xB
*/
    out_vectors.clear();
    int last_equip_id = -1;
    size_t i = 0, j = 0;
    for (j = 0; j < type_ids.size(); j++) {
        if (j != 0 && last_equip_id != get_equip_id(type_ids[j])) {
            out_vectors.push_back(IdVector(type_ids.begin() + i,
                                            type_ids.begin() + j));
            i = j;
        }
        last_equip_id = get_equip_id(type_ids[j]);
    }
    if (i != j) {
        out_vectors.push_back(IdVector(type_ids.begin() + i,
                                        type_ids.begin() + j));
    }
    return;
}

static bool
generate_log_config_msgs(FILE * DiagCfgFile, vector<string> type_names){
	IdVector type_ids;
	BinaryBuffer buf;

	bool success = map_typenames_to_ids(type_names, type_ids);
	if (!success) {
		return false;
	}


    // Yuanjie: check if Modem_debug_message exists. If so, enable it in slightly different way
    IdVector::iterator debug_ind = type_ids.begin();
    for(; debug_ind != type_ids.end(); debug_ind++) {
        if(*debug_ind==Modem_debug_message){
            break;
        }
    }
    if(debug_ind!=type_ids.end()){
        //Modem_debug_message should be enabled
        type_ids.erase(debug_ind);

        // Enable WCDMA debug
        buf = encode_log_config(DEBUG_WCDMA_L1, type_ids);
        if (buf.first != NULL && buf.second != 0) {
            (void) send_msg(DiagCfgFile, buf.first, buf.second);
            delete [] buf.first;
        } else {
            return false;
        }
    }

    // send log config messages
    std::vector<IdVector> type_id_vectors;
    sort_type_ids(type_ids, type_id_vectors);
    for (size_t i = 0; i < type_id_vectors.size(); i++) {
        const IdVector& v = type_id_vectors[i];
        buf = encode_log_config(SET_MASK, v);
        if (buf.first != NULL && buf.second != 0) {
            (void) send_msg(DiagCfgFile, buf.first, buf.second);
            delete [] buf.first;
        } else {
            return false;
        }
    }

    return true;

}

static bool
send_msg (FILE * DiagCfgFile, const char *b, int length) {

	string frame = encode_hdlc_frame(b, length);
    //fputs(frame.data(), DiagCfgFile);
	fwrite (frame.data() , sizeof(char), frame.size(), DiagCfgFile);
    //cout << "Writing" << frame.c_str() << " res: "<< res << strerror(errno) << endl;
    fflush(DiagCfgFile);

    return true;
}

static bool
generate_log_config_headers( FILE * DiagCfgFile ){
	BinaryBuffer buf;
	IdVector empty;
	for (int k = 0; k < 12; k++) {
		switch (k) {
			case 0:
				buf = encode_log_config(DIAG_BEGIN_1D, empty);
				break;
			case 1:
				buf = encode_log_config(DIAG_BEGIN_00, empty);
				break;
			case 2:
				buf = encode_log_config(DIAG_BEGIN_7C, empty);
				break;
			case 3:
				buf = encode_log_config(DIAG_BEGIN_1C, empty);
				break;
			case 4:
				buf = encode_log_config(DIAG_BEGIN_0C, empty);
				break;
			case 5:
				buf = encode_log_config(DIAG_BEGIN_63, empty);
				break;
			case 6:
				buf = encode_log_config(DIAG_BEGIN_4B0F0000, empty);
				break;
			case 7:
				buf = encode_log_config(DIAG_BEGIN_4B090000, empty);
				break;
			case 8:
				buf = encode_log_config(DIAG_BEGIN_4B080000, empty);
				break;
			case 9:
				buf = encode_log_config(DIAG_BEGIN_4B080100, empty);
				break;
			case 10:
				buf = encode_log_config(DIAG_BEGIN_4B040000, empty);
				break;
			case 11:
				buf = encode_log_config(DIAG_BEGIN_4B040F00, empty);
				break;
			default:
				break;
		}
		if (buf.first != NULL && buf.second != 0) {
			(void) send_msg(DiagCfgFile, buf.first, buf.second);
			delete [] buf.first;
			buf.first = NULL;
		} else {
			cout << "Log config msg failed to encode\n";
			return false;
		}
	}
	return true;
}

// A helper function that generated binary code of config ends.
static bool
generate_log_config_ends (FILE * DiagCfgFile ) {
    BinaryBuffer buf;
    IdVector empty;
    buf = encode_log_config(DIAG_END_6000, empty);
    if (buf.first != NULL && buf.second != 0) {
        (void) send_msg(DiagCfgFile, buf.first, buf.second);
        delete [] buf.first;
        buf.first = NULL;
    } else {
        return false;
    }
    return true;
}

pair<int,string> generate_diag_cfg (string filepath, vector<string> msgTypes) {
	map<string,int>::iterator itt;
	BinaryBuffer buf;
	IdVector empty;

	bool success;

	for(string curType:msgTypes){
		if( !typename_exist(curType.c_str()) ) return make_pair(-1,"unknown message type: " + curType);
	}


	FILE * DiagCfgFile = fopen (filepath.c_str(),"wb");

	if (DiagCfgFile == NULL) return make_pair(-1,"unable to open file:" + filepath);


	// Config Headers
	success = generate_log_config_headers( DiagCfgFile );
	if (!success) {
		fclose(DiagCfgFile);
		return make_pair(-1,"failed to generate log config headers" + filepath);
	}

	// Disable previous logs
	for (int k = 0; k < 2; k++) {
		if (k == 0) {   // Disable normal log msgs
			buf = encode_log_config(DISABLE_DEBUG, empty);
		} else {        // Disable debug msgs
			buf = encode_log_config(DISABLE, empty);
		}
		if (buf.first != NULL && buf.second != 0) {
			(void) send_msg(DiagCfgFile, buf.first, buf.second);
			delete [] buf.first;
			buf.first = NULL;
		} else {
			fclose(DiagCfgFile);
			return make_pair(-1,"failed to previous logs");
		}
	}

	success = generate_log_config_msgs(DiagCfgFile, msgTypes);
	if (!success) {
		fclose(DiagCfgFile);
		return make_pair(-1,"failed to write log config messages");
	}

	// Config End
	success = generate_log_config_ends(DiagCfgFile);
	if (!success) {
		fclose(DiagCfgFile);
		return make_pair(-1,"failed to write log config ends");
	}

    fseek(DiagCfgFile,0,SEEK_END);
    int size = ftell(DiagCfgFile);

	fclose(DiagCfgFile);
    stringstream myresult;
    myresult << "successfully written " << size << "bytes";
	return make_pair(0, myresult.str().c_str());
}


// Return: successful or not

string* disable_logs_command(int id) {
	IdVector empty;
	BinaryBuffer buf;

    if(id == 0) {
        buf = encode_log_config(DISABLE_DEBUG, empty);
    }else{
        buf = encode_log_config(DISABLE, empty);
    }
	if (buf.first == NULL || buf.second == 0) {
		return NULL;
	}

	string frame = encode_hdlc_frame(buf.first, buf.second);
    delete [] buf.first;
    return &frame;

}

















/*
int getSupportedTypeByName(string str){

    itt = SUPPORTED_TYPES.find(str);

    if (itt != SUPPORTED_TYPES.end()) {
    	return SUPPORTED_TYPES.find(str)->second;
    }else{
    	return 0;
    }
}
*/
