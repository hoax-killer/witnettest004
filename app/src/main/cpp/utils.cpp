/*
 * utils.cpp
 *
 *  Created on: Mar 12, 2018
 *      Author: siralex
 */


#include "utils.h"
#include <cstring>
#include <string>


int
find_ids (const char *name, IdVector& out_vector) {
    int cnt = 0;
    for (int i = 0; i < LogPacketTypeID_To_Name.size(); i++) {
        if (strcmp(name, LogPacketTypeID_To_Name[i].name) == 0) {
            out_vector.push_back(LogPacketTypeID_To_Name[i].val);
            cnt++;
        }
    }
    return cnt;
}


const char*
search_name (const ValueName id_to_name [], int n, int val) {

    for (int i = 0; i < n; i++) {
        if (id_to_name[i].val == val) {
            return id_to_name[i].name;
        }
    }
    return NULL;
}
/*

 *
 *
 *
 *
 *
const char*
search_name (int val) {
    for (int i = 0; i < LogPacketTypeID_To_Name.size(); i++) {
        if (LogPacketTypeID_To_Name[i].val == val) {
            return LogPacketTypeID_To_Name[i].name;
        }
    }
    return NULL;
}
*/

bool
typename_exist( const char *name ){
	for (int i = 0; i < LogPacketTypeID_To_Name.size(); i++) {
		if (strcmp(name, LogPacketTypeID_To_Name[i].name) == 0) {
	            return true;
		}
	}
	return false;
}

constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string hexStr(unsigned char *data, int len)
{
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }
    return s;
}