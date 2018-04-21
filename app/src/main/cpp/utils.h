/*
 * utils.h
 *
 *  Created on: Mar 12, 2018
 *      Author: siralex
 */

#ifndef UTILS_H_
#define UTILS_H_
#include <android/log.h>
#include "consts.h"
#define  LOG_TAG    "witnettest"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define ARRAY_SIZE(array, element_type) (sizeof(array) / sizeof(element_type))

int find_ids (const char *name, IdVector& out_vector);
const char* search_name (const ValueName id_to_name [], int n, int val);
bool typename_exist( const char *name );

#endif /* UTILS_H_ */
