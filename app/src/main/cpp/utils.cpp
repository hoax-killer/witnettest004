/*
 * utils.cpp
 *
 *  Created on: Mar 12, 2018
 *      Author: siralex
 */


#include "utils.h"
#include <cstring>


// Find multiple IDs coressponding to name, and append all IDs to out_vector
// Return how many IDs are found
// в текущий момент имеется всего два элемента с одинаковым именем и разными ID
// аналогично всего 2 элемента содержат одинаковый ID при разных именах
// и только из за них приходится отказатьс от использования map
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
search_name (int val) {
    for (int i = 0; i < LogPacketTypeID_To_Name.size(); i++) {
        if (LogPacketTypeID_To_Name[i].val == val) {
            return LogPacketTypeID_To_Name[i].name;
        }
    }
    return NULL;
}


bool
typename_exist( const char *name ){
	for (int i = 0; i < LogPacketTypeID_To_Name.size(); i++) {
		if (strcmp(name, LogPacketTypeID_To_Name[i].name) == 0) {
	            return true;
		}
	}
	return false;
}
