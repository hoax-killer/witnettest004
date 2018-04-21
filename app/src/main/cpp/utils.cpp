/*
 * utils.cpp
 *
 *  Created on: Mar 12, 2018
 *      Author: siralex
 */


#include "utils.h"
#include <cstring>


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
