//
// Created by siralex on 27-May-18.
//

#ifndef WITNETTEST004_WS_WRAP_H
#define WITNETTEST004_WS_WRAP_H

void initialize_epan(void);
std::string process_message( unsigned char* indata, unsigned int datasize);
void stop_epan(void);

#endif //WITNETTEST004_WS_WRAP_H
