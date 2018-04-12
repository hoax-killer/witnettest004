//
// Created by siralex on 27-Mar-18.
//

#ifndef WITNETTEST004_HDLC_H
#define WITNETTEST004_HDLC_H

#include <string>

std::string encode_hdlc_frame (const char *payld, int length);
void  feed_binary (const char *b, int length);
bool get_next_frame (std::string& output_frame, bool& crc_correct);

#endif //WITNETTEST004_HDLC_H
