/*
 * utils.h
 *
 *  Created on: Mar 12, 2018
 *      Author: siralex
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "consts.h"

int find_ids (const char *name, IdVector& out_vector);
const char* search_name (int val);
bool typename_exist( const char *name );

#endif /* UTILS_H_ */
