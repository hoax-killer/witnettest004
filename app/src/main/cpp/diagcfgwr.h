/*
 * diagcfgwr.h
 *
 *  Created on: Mar 7, 2018
 *      Author: siralex
 */

#ifndef DIAGCFGWR_H_
#define DIAGCFGWR_H_

#include <iostream>
#include <vector>
#include <map>

using namespace std;

//int getSupportedTypeByName(string str);
pair<int, string> generate_diag_cfg (string filepath, vector<string> msgTypes);

#endif /* DIAGCFGWR_H_ */
