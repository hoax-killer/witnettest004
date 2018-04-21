//
// Created by siralex on 11-Apr-18.
//

#ifndef WITNETTEST004_LOG_PACKET_UTILS_H
#define WITNETTEST004_LOG_PACKET_UTILS_H

#include <cstring>
#include <ctime>
#include <vector>
#include "log_packet.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

static int
_decode_by_fmt (const Fmt fmt [], int n_fmt,
                const char *b, int offset, int length,
                json &j) {

    int n_consumed = 0;

    for (int i = 0; i < n_fmt; i++) {
        const char *p = b + offset + n_consumed;
        switch (fmt[i].type) {
            case UINT:
            {
                unsigned int ii = 0;
                unsigned long long iiii = -1LL;
                switch (fmt[i].len) {
                    case 1:
                        ii = *((unsigned char *) p);
                        break;
                    case 2:
                        ii = *((unsigned short *) p);
                        break;
                    case 4:
                        ii = *((unsigned int *) p);
                        break;
                    case 8:
                    {
                        // iiii = *((unsigned long long *) p);
                        unsigned char buffer64[256] = {0};
                        memcpy(buffer64, p, sizeof(unsigned long long));
                        iiii = *reinterpret_cast<unsigned long long *>(buffer64);
                        break;
                    }
                    default:
                        break;
                }
                // Convert to a Python integer object or a Python long integer object
                // TODO: make it little endian

                //memset(myhex,0,512);

                if (fmt[i].len <= 4){
                    //sprintf(myhex, "%02x", ii);
                    j[fmt[i].field_name] = ii;
                }else{
                    //sprintf(myhex, "%02x", iiii);
                    j[fmt[i].field_name] = iiii;
                }
                //decoded = myhex;


                n_consumed += fmt[i].len;
                break;
            }
/*
            case BYTE_STREAM:
            {
                char hex[10] = {};
                std::string ascii_data = "0x";
                for (int k = 0; k < fmt[i].len; k++) {
                    sprintf(hex, "%02x", p[k] & 0xFF);
                    ascii_data += hex;
                }
                decoded = Py_BuildValue("s", ascii_data.c_str());
                n_consumed += fmt[i].len;
                break;
            }

            case BYTE_STREAM_LITTLE_ENDIAN:
            {
                assert(fmt[i].len > 0);
                char hex[10] = {};
                std::string ascii_data = "0x";
                for (int k = fmt[i].len - 1; k >= 0; k--) {
                    sprintf(hex, "%02x", p[k] & 0xFF);
                    ascii_data += hex;
                }
                decoded = Py_BuildValue("s", ascii_data.c_str());
                n_consumed += fmt[i].len;
                break;
            }

            case PLMN_MK1:
            {
                assert(fmt[i].len == 6);
                const char *plmn = p;
                decoded = PyString_FromFormat("%d%d%d-%d%d%d",
                                              plmn[0],
                                              plmn[1],
                                              plmn[2],
                                              plmn[3],
                                              plmn[4],
                                              plmn[5]);
                n_consumed += fmt[i].len;
                break;
            }

            case PLMN_MK2:
            {
                assert(fmt[i].len == 3);
                const char *plmn = p;
                decoded = PyString_FromFormat("%d%d%d-%d%d",
                                              plmn[0] & 0x0F,
                                              (plmn[0] >> 4) & 0x0F,
                                              plmn[1] & 0x0F,
                                              plmn[2] & 0x0F,
                                              (plmn[2] >> 4) & 0x0F);
                // MNC can have two or three digits
                int last_digit = (plmn[1] >> 4) & 0x0F;
                if (last_digit < 10)    // last digit exists
                    PyString_ConcatAndDel(&decoded, PyString_FromFormat("%d", last_digit));
                n_consumed += fmt[i].len;
                break;
            }
*/
            case QCDM_TIMESTAMP:
            {
                // in old dok 80-V1294 time counter has 6 bytes to count with 1.25ms resolution
                // and 2 low bytes to count within 1.25 ms with rollover on 49152
                // so we have one ms corresponding 39321 ticks => 1s ~ 39000000 ticks. Why 52 here?

                //const double PER_SECOND = 52428800.0;
                //const double PER_USECOND = 52428800.0 / 1.0e6;



                unsigned long long iiii = *((unsigned long long *) p);

                //int seconds = int(double(iiii) / PER_SECOND);
                //int useconds = (double(iiii) / PER_USECOND) - double(seconds) * 1.0e6;

                const unsigned long PER_MSECOND = 52428800 / 1000;
                unsigned long long mseconds = iiii / PER_MSECOND;
                unsigned long long epoch_gps_shift = 315964800000; //ms  between UNIX epoch and GPS epoch



                //PyObject *epoch = PyDateTime_FromDateAndTime(1980, 1, 6, 0, 0, 0, 0);
                //PyObject *delta = PyDelta_FromDSU(0, seconds, useconds);
                //decoded = PyNumber_Add(epoch, delta);

                //Py_DECREF(epoch);
                //Py_DECREF(delta);

                //memset(myhex,0,512);
                //sprintf(myhex, "%lu", epoch_gps_shift+mseconds);
                //decoded = myhex;
                j[fmt[i].field_name] = epoch_gps_shift+mseconds;

                n_consumed += fmt[i].len;
                break;
            }
/*
            case BANDWIDTH:
            {
                assert(fmt[i].len == 1);
                unsigned int ii = *((unsigned char *) p);
                decoded = PyString_FromFormat("%d MHz", ii / 5);
                n_consumed += fmt[i].len;
                break;
            }

            case RSRP:
            {
                // (0.0625 * x - 180) dBm
                assert(fmt[i].len == 2);
                short val = *((short *) p);
                decoded = Py_BuildValue("f", val * 0.0625 - 180);
                n_consumed += fmt[i].len;
                break;
            }

            case RSRQ:
            {
                // (0.0625 * x - 30) dB
                assert(fmt[i].len == 2);
                short val = *((short *) p);
                decoded = Py_BuildValue("f", val * 0.0625 - 30);
                n_consumed += fmt[i].len;
                break;
            }

            case WCDMA_MEAS:
            {   // (x-256) dBm
                assert(fmt[i].len == 1);
                unsigned int ii = *((unsigned char *) p);
                decoded = Py_BuildValue("i", (int)ii-256);
                n_consumed += fmt[i].len;
                break;
            }
*/
            case SKIP:
                n_consumed += fmt[i].len;
                break;

            case PLACEHOLDER:
            {
                // it is only name just now, later it would be filled from other fields etc
                j[fmt[i].field_name] = 0;
                break;
            }

            default:
                break;
        }

    }

    return n_consumed;
}

#endif //WITNETTEST004_LOG_PACKET_UTILS_H
