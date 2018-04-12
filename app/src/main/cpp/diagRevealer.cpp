//
// Created by siralex on 21-Mar-18.
//

#include <jni.h>
#include <android/log.h>
#include <string.h>
#include <endian.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <sstream>
#include <errno.h>
#include "consts.h"
#include "diagcfgwr.h"
#include "log_packet.h"
#include "hdlc.h"
#include <poll.h>

#define  LOG_TAG    "witnettest"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

/* Different IOCTL values */
#define DIAG_IOCTL_COMMAND_REG  	0
#define DIAG_IOCTL_COMMAND_DEREG	1
#define DIAG_IOCTL_SWITCH_LOGGING	7
#define DIAG_IOCTL_GET_DELAYED_RSP_ID 	8
#define DIAG_IOCTL_LSM_DEINIT		9
#define DIAG_IOCTL_DCI_INIT		20
#define DIAG_IOCTL_DCI_DEINIT		21
#define DIAG_IOCTL_DCI_SUPPORT		22
#define DIAG_IOCTL_DCI_REG		23
#define DIAG_IOCTL_DCI_STREAM_INIT	24
#define DIAG_IOCTL_DCI_HEALTH_STATS	25
#define DIAG_IOCTL_DCI_LOG_STATUS	26
#define DIAG_IOCTL_DCI_EVENT_STATUS	27
#define DIAG_IOCTL_DCI_CLEAR_LOGS	28
#define DIAG_IOCTL_DCI_CLEAR_EVENTS	29
#define DIAG_IOCTL_REMOTE_DEV		32
#define DIAG_IOCTL_VOTE_REAL_TIME	33
#define DIAG_IOCTL_GET_REAL_TIME	34
#define DIAG_IOCTL_PERIPHERAL_BUF_CONFIG	35
#define DIAG_IOCTL_PERIPHERAL_BUF_DRAIN		36
#define DIAG_IOCTL_REGISTER_CALLBACK	37
#define DIAG_IOCTL_HDLC_TOGGLE	38

#define USB_MODE		1
#define MEMORY_DEVICE_MODE		2
#define NO_LOGGING_MODE			3
#define CALLBACK_MODE		    6
#define TTY_MODE	            8

#define BUFFER_SIZE	65536

int fd;

/*
 * MDM VS. MSM
 * Reference: https://android.googlesource.com/kernel/msm.git/+/android-6.0.0_r0.9/include/linux/diagchar.h
 */
enum remote_procs {
    MSM     = 0,
    MDM     = 1,
    MDM2    = 2,
    QSC     = 5,
};


/* Raw binary data type
 * Reference: https://android.googlesource.com/kernel/msm.git/+/android-6.0.0_r0.9/include/linux/diagchar.h
 */
#define MSG_MASKS_TYPE		    0x00000001
#define LOG_MASKS_TYPE		    0x00000002
#define EVENT_MASKS_TYPE	    0x00000004
#define PKT_TYPE		        0x00000008
#define DEINIT_TYPE		        0x00000010
#define USER_SPACE_DATA_TYPE	0x00000020
#define DCI_DATA_TYPE		    0x00000040
#define CALLBACK_DATA_TYPE	    0x00000080
#define DCI_LOG_MASKS_TYPE	    0x00000100
#define DCI_EVENT_MASKS_TYPE	0x00000200
#define DCI_PKT_TYPE		    0x00000400

#define FIFO_MSG_TYPE_LOG 1
#define FIFO_MSG_TYPE_START_LOG_FILE 2
#define FIFO_MSG_TYPE_END_LOG_FILE 3

/*
 * Android 7.0: switch_logging_mode structure
 * Reference: https://android.googlesource.com/kernel/msm.git/+/android-7.1.0_r0.3/drivers/char/diag/diagchar.h
 */
struct diag_logging_mode_param_t {
    uint32_t req_mode;
    uint32_t peripheral_mask;
    uint8_t mode_param;
} __packed;
#define DIAG_CON_APSS		(0x0001)	/* Bit mask for APSS */
#define DIAG_CON_MPSS		(0x0002)	/* Bit mask for MPSS */
#define DIAG_CON_LPASS		(0x0004)	/* Bit mask for LPASS */
#define DIAG_CON_WCNSS		(0x0008)	/* Bit mask for WCNSS */
#define DIAG_CON_SENSORS	(0x0010)	/* Bit mask for Sensors */
#define DIAG_CON_NONE		(0x0000)	/* Bit mask for No SS*/
#define DIAG_CON_ALL		(DIAG_CON_APSS | DIAG_CON_MPSS \
				| DIAG_CON_LPASS | DIAG_CON_WCNSS \
				| DIAG_CON_SENSORS)

char buf_read[BUFFER_SIZE] = {};
char buf_send[BUFFER_SIZE] = {};
//static int mode = MEMORY_DEVICE_MODE;   // logging mode
static uint16_t remote_dev = 0;         // MSM (0) or not
int client_id;	                        // DCI client ID (allocated by diag driver)
uint16_t peripheral = remote_dev;

typedef struct {
    char *p;
    size_t len;
} binaryBuffer;

volatile bool must_stop = false;
bool io_timeout_reached = false;

jobject NewInteger(JNIEnv* env, int value);
bool _diag_switch_logging(int fd, int log_mode);

void intr_handler(int sig){
    LOGD("Inerrupt triggered\n");
}

static unsigned long long
get_posix_timestamp () {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long mslong = (unsigned long long) tv.tv_sec * 1000L + tv.tv_usec / 1000;
    //unsigned long long mslong = 1468496250207L;
    return  mslong;

}


extern "C"
JNIEXPORT jobjectArray JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_writeDiagCfg(
        JNIEnv *env,
        jobject obj,
        jobjectArray stringArray,
        jstring filepath){

    int stringCount = env->GetArrayLength(stringArray);
    vector<string> reqMsgTypes;
    string filePath;

    for (int i=0; i<stringCount; i++) {
        jstring string = (jstring) (env->GetObjectArrayElement(stringArray, i));
        reqMsgTypes.push_back(env->GetStringUTFChars(string, 0));
        // Don't forget to call `ReleaseStringUTFChars` when you're done.
    }

    filePath = env->GetStringUTFChars( filepath , NULL );

    LOGD("DEBUGNET: trying to write Diag.cfg to path %s", filePath.c_str());
    pair<int,string> ret = generate_diag_cfg(filePath, reqMsgTypes);

    string respond = "DEBUGNET: result is " + ret.second + "\n";
    LOGD("%s",respond.c_str());

    jobjectArray retobjarr= (jobjectArray)env->NewObjectArray(2,env->FindClass("java/lang/Object"),NULL);
    env->SetObjectArrayElement(retobjarr,0,NewInteger(env, ret.first));
    env->SetObjectArrayElement(retobjarr,1,env->NewStringUTF(ret.second.c_str()));

    return retobjarr;
}

extern "C"
JNIEXPORT void JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_processLogChunk(
        JNIEnv *env,
        jobject obj,
        jbyteArray pktbytes){

    char* payload_buf = (char*)env->GetByteArrayElements( pktbytes, NULL);
    int buf_length = env->GetArrayLength(pktbytes);


// we don't know how many network messages can be passed within one DIAG message
// we also don't know that the boundary of DIAG message is valid HDLC boundary
// that is why we push all DIAG messages is one buffer and extract network packets from there
// using HDLC boundaries

    feed_binary(payload_buf, buf_length);

    string frame;
    bool crc_correct    = false;

    while(get_next_frame(frame, crc_correct)){
        if ( !crc_correct ){
            LOGD("wrong HDLC frame CRC\n");
            continue;
        }

        if(is_log_packet(frame.data(), frame.size())){
            LOGD("IS LOG PACKET\n");
            const char *s = frame.data();
// https://android.googlesource.com/kernel/msm.git/+/android-7.1.0_r0.3/drivers/char/diag/diag_dci.c
// функция extract_dci_log
// 8 = 2 (0x1000) + 2 (len1) + 2 (log_msg_len) + 2 (type_id)
            string decoded = decode_log_packet(s + 2,  frame.size() - 2, false);

        }else{
            LOGD("NOT LOG PACKET\n");
        }

    }


    env->ReleaseByteArrayElements(pktbytes, (jbyte*)payload_buf, 0);
}



extern "C"
JNIEXPORT jobjectArray JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_getTypeNames(
        JNIEnv *env,
        jobject obj ){

    jobjectArray ret;

    ret= (jobjectArray)env->NewObjectArray(LogPacketTypeID_To_Name.size(),env->FindClass("java/lang/String"),NULL);

    for(int i=0; i<LogPacketTypeID_To_Name.size(); i++)
        env->SetObjectArrayElement(ret,i,env->NewStringUTF(LogPacketTypeID_To_Name[i].name));

    return ret;
}


static binaryBuffer
read_diag_cfg (const char *filename)
{
    binaryBuffer ret;
    size_t file_sz = 0;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        //perror("Error");
        LOGD("Unable to open diag.cfg\n");
        goto fail;
    }
    fseek(fp, 0L, SEEK_END);
    file_sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if (file_sz > 0 && file_sz <= BUFFER_SIZE) {
        ret.p = (char *) malloc(file_sz);
        if (ret.p == NULL) {
            //fprintf(stderr, "Error: Failed to malloc.\n");
            LOGD("Error: Failed to malloc.\n");
            goto fail;
        }
        ret.len = file_sz;
        int retcode = fread(ret.p, sizeof(char), ret.len, fp);
        if (retcode != ret.len) {
            //perror("Error");
            LOGD("Error: unable to read diag.cfg\n");
            free(ret.p);
            goto fail;
        }
    } else {
        //fprintf(stderr, "Error: File size inappropriate.\n");
        LOGD("Error: File size inappropriate for diag.cfg\n");
        goto fail;
    }

    LOGD("Readinf diag.cfg OK\n");
    return ret;

    fail:
    ret.p = NULL;
    ret.len = 0;
    return ret;
}

// Write commands to /dev/diag device.
static int
write_commands (int fd, binaryBuffer *pbuf_write)
{
    size_t i = 0;
    char *p = pbuf_write->p;

    //Set fd to non-blocking mode, though it is already should be
    int flags = fcntl(fd, F_GETFL, 0);
    int fret = fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    //Buffer to mask command
    char *send_buf = (char *) malloc(pbuf_write->len + 10);
    if (send_buf == NULL) {
        //perror("Error");
        LOGD("malloc failed in write_commands\n");
        return -1;
    }

    // Metadata for each mask command
    size_t offset = remote_dev ? 8 : 4; //offset of the metadata (one int for MSM, 2 ints for MDM)
    LOGD("write_commands: offset=%d remote_dev=%d\n",offset,remote_dev);
    *((int *)send_buf) = htole32(USER_SPACE_DATA_TYPE);
    if(remote_dev){
        /*
          * MDM device: should let diag driver know it
          * Reference: diag_get_remote and diagchar_write
          * in https://android.googlesource.com/kernel/msm.git/+/android-6.0.0_r0.9/drivers/char/diag/diagchar_core.c
          */
        *((int *)send_buf+1) =  - MDM; // here is the second int for MDM
    }

// i 	current position in Diag.cfg
// len 	length of current command from Diag.cfg
// p	buffer containing Diag.cfg
    while (i < pbuf_write->len) {
        size_t len = 0;
        while (i + len < pbuf_write->len && p[i + len] != 0x7e) len++;
        if (i + len >= pbuf_write->len)
            break;
        len++;
        if (len >= 3) {
            memcpy(send_buf + offset, p + i, len);
            fflush(stdout);
            int ret = write(fd, (const void *) send_buf, len + offset);
            if (ret < 0) {
                LOGD("write_commands error (len=%d, offset=%d): %s\n", len, offset, strerror(errno));
                return -1;
            }

            /*
             * Read responses after writting each command.
             * NOTE: This step MUST EXIST. Without it, some phones cannot collect logs for two reasons:
             *  (1) Ensure every config commands succeeds (otherwise read() will be blocked)
             *  (2) Clean up the buffer, thus avoiding pollution of later real cellular logs
             */
            int read_len = read(fd, buf_read, sizeof(buf_read));


            if (read_len < 0) {
                LOGD("write_commands read error: %s\n", strerror(errno));
                return -1;
            } else {
                LOGD("Reading %d bytes of resp\n", read_len);
            }
        }
        i += len;
    }

    return 0;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_readDiag(
        JNIEnv *env,
        jobject obj, jstring diagConfigFilePath) {

    must_stop = false;

    int ret;
    std::stringstream myresult;

    jobjectArray retobjarr= (jobjectArray)env->NewObjectArray(2,env->FindClass("java/lang/Object"),NULL);

    const char * diagConfigPath = env->GetStringUTFChars( diagConfigFilePath , NULL );
    fd = open("/dev/diag", O_RDWR|O_LARGEFILE|O_NONBLOCK|O_NOCTTY);

    if (fd < 0) {
        env->SetObjectArrayElement(retobjarr,0,NewInteger(env, -1));
        env->SetObjectArrayElement(retobjarr,1,env->NewStringUTF("open diag dev failed"));
        return retobjarr;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    int fret = fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    LOGD("open diag dev ok, response %d\n", fd);


    bool resp = _diag_switch_logging(fd, MEMORY_DEVICE_MODE);
    if(!resp){
        myresult <<  "All attempts to SWITCH_LOGGING failed ";
        LOGD("DIAG_IOCTL_SWITCH_LOGGING failed");
        env->SetObjectArrayElement(retobjarr,0,NewInteger(env, -1));
        env->SetObjectArrayElement(retobjarr,1,env->NewStringUTF(myresult.str().c_str()));
        close(fd);
        return retobjarr;
    }


    LOGD("trying to read Diag.cfg\n");
    binaryBuffer buf_write = read_diag_cfg( diagConfigPath );
    LOGD("reading Diag.cfg from %s\n", diagConfigPath);

    if (buf_write.p == NULL || buf_write.len == 0) {
        myresult <<  "reading Diag.cfg failed " << "end";
        LOGD("reading Diag.cfg failed\n");
        env->SetObjectArrayElement(retobjarr,0,NewInteger(env, -1));
        env->SetObjectArrayElement(retobjarr,1,env->NewStringUTF(myresult.str().c_str()));
        close(fd);
        return retobjarr;
    }

    LOGD("Diag.cfg has been read OK, trying to send it to diag port\n");

    ret = write_commands(fd, &buf_write);
    fflush(stdout);
    free(buf_write.p);
    if (ret != 0) {
        myresult <<  "writing Diag.cfg failed " << ret << "end";
        LOGD("writing Diag.cfg failed with result %d\n", ret);
        env->SetObjectArrayElement(retobjarr,0,NewInteger(env, -1));
        env->SetObjectArrayElement(retobjarr,1,env->NewStringUTF(myresult.str().c_str()));
        close(fd);
        return retobjarr;
    }

    jclass cls = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(cls, "logRevealer", "([B[B)V");

    LOGD("writing Diag.cfg OK, trying to read logs\n");

    struct pollfd  fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    //int result;
   //fd_set readset;
   //FD_ZERO(&readset);
   //FD_SET(fd, &readset);

    //struct sigaction new_action;
    //new_action.sa_handler=intr_handler;
    //sigemptyset(&new_action.sa_mask);
    //new_action.sa_flags=0;
    //sigaction(SIGALRM, &new_action, 0);
    //alarm(0);
// {[4b USDT][4b num_data][? 4b][4b msg_len]|payload>>[data]}
    while(1){

        ret = poll(fds,1,0);
        if(fds[0].revents & POLLIN){
            //LOGD("enter blocking read");
            // this request is blocking
            //alarm(10);
            int read_len = read(fd, buf_read, sizeof(buf_read));
            //alarm(0);
            //LOGD("exiting blocking read with data %i\n", read_len);
            if (read_len > 0) {
                if (*((int *)buf_read) == USER_SPACE_DATA_TYPE) {
                    int num_data = *((int *)(buf_read + 4));
                    int i = 0;
                    long long offset = remote_dev ? 12 : 8;
                    int send_offset = 0;

                    short fifo_msg_type = FIFO_MSG_TYPE_LOG;
                    short fifo_msg_len;
                    unsigned long long ts = get_posix_timestamp();
                    char buf_head[12] = {};
                    //LOGD("revealer got submessages num. %i\n", num_data);
                    for (i = 0; i < num_data; i++) {
                        // apparently subpacket consist of two fields: 4 bytes msg_len and message
                        //read msg_len
                        int msg_len = 0;
                        memcpy(&msg_len, buf_read + offset, sizeof(int));
                        if (msg_len < 0) continue;


                        // Write payload to pipe
                        // +4 for msg_len
                        memcpy(buf_send+send_offset, buf_read + offset + 4, msg_len);


                        offset += msg_len + 4;
                        send_offset += msg_len;

                        //LOGD("revealer got submessage. size: %i\n", msg_len);
                    }
                    // actually we make buf_send from incoming messages excluding all headers
                    // I presume we won't need headers because data is HDLC encoded
                    // now we form our own header for this combined buffer
                    //LOGD("revealer got message. size: %i\n", send_offset);
                    short head_offset = 0;
                    memcpy(buf_head+head_offset, &fifo_msg_type, sizeof(short));
                    head_offset+=sizeof(short);

                    // Write size of (payload + timestamp)
                    fifo_msg_len = (short) send_offset + 8;
                    memcpy(buf_head+head_offset, &fifo_msg_len, sizeof(short));
                    head_offset+=sizeof(short);

                    memcpy(buf_head+head_offset, &ts, sizeof(unsigned long long));
                    head_offset+=sizeof(unsigned long long);

                    jbyteArray headerArray  = env->NewByteArray(head_offset);
                    env->SetByteArrayRegion(headerArray, 0, head_offset, (const jbyte *) buf_head);

                    jbyteArray dataArray    = env->NewByteArray(send_offset);
                    env->SetByteArrayRegion(dataArray, 0, send_offset, (const jbyte *) buf_send);

                    env->CallVoidMethod(obj, mid, headerArray, dataArray);

                    env->DeleteLocalRef(headerArray);
                    env->DeleteLocalRef(dataArray);

/*
                    jbyteArray bArray = env->NewByteArray(send_offset);
                    env->SetByteArrayRegion(bArray, 0, send_offset, (const jbyte *) buf_send);
                    env->CallVoidMethod(obj, mid, bArray);

                    env->DeleteLocalRef(bArray);
*/

                    //LOGD("packet been sent to FIFO. size: %i\n", send_offset);

                }else{
                    // TODO: Check other raw binary types
                    // LOGI("Not USER_SPACE_DATA_TYPE: %d\n", *((int *)buf_read));
                }
            }
        }


        if(must_stop){
            must_stop = false;
            myresult <<  "STOP command received";
            break;
        }

    }

    //TODO наверное вместо этих команд, которые всё равно не останавливают лог надо
    // через ioctl вызывть функц. diag_state_close_smd() или из memory_device_mode в no_logging_mode?

    LOGD("Trying to stop logs\n");
    char  tmpbuf[64];
    // two commands: DISABLE_DEBUG and DISABLE
    for(int comm_id = 0; comm_id < 2; comm_id ++){
        string* framep = disable_logs_command( comm_id);
        if( framep == NULL){
            LOGD("Failed to stop  logging. Encoding failed.\n");
            //myresult <<  " \nFailed to stop  logging. Encoding failed.";
            break;
        }

        string frame = *framep;

        *((int *)tmpbuf) = htole32(USER_SPACE_DATA_TYPE); // header
        memcpy(tmpbuf+4, frame.data(),frame.size() ); // command
        ret = write (fd,  tmpbuf, frame.size());

        if(ret<0) {
            LOGD("Stop logging error (err=%d): %s\n", ret, strerror(errno));
            //myresult <<  " \nFailed to stop logging!";
            break;
        }
        if((ret >= 0) && (comm_id == 1)){
            LOGD("Stop logging commands succeed\n");
            //myresult <<  " \nStop logging commands succeed.";
        }
    }
/*
     _diag_switch_logging(fd, USB_MODE);


    while(fds[0].revents & POLLIN){

        int read_len = read(fd, buf_read, sizeof(buf_read));

        if (read_len < 0) {
            LOGD("write_commands read error: %s\n", strerror(errno));
            break;
        } else {
            LOGD("Reading %d bytes of resp\n", read_len);
        }
        usleep(100000);
    }
*/


    env->SetObjectArrayElement(retobjarr,0,NewInteger(env, 0));
    env->SetObjectArrayElement(retobjarr,1,env->NewStringUTF(myresult.str().c_str()));

    ret = close(fd);
    if(ret<0) {
        LOGD("closefile error (err=%d): %s\n", ret, strerror(errno));

    }else{
        LOGD("closefile OK ");
    }

    return retobjarr;

}

extern "C"
JNIEXPORT void JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_stopDiag(
        JNIEnv *env,
        jobject obj) {
    if(!must_stop) must_stop = true;



}

jobject NewInteger(JNIEnv* env, int value)
{
    jclass integerClass = env->FindClass("java/lang/Integer");
    jmethodID integerConstructor = env->GetMethodID(integerClass, "<init>", "(I)V");
    return env->NewObject(integerClass, integerConstructor, static_cast<jint>(value));
}

int _stop_logging(int fd){
    // get prepared end hdlc encoded STOP command

}

bool _diag_switch_logging(int fd, int log_mode){
    int ret;
    int cur_mode = log_mode;

    ret = ioctl(fd, DIAG_IOCTL_SWITCH_LOGGING, &cur_mode);
    if (ret < 0) {
        LOGD("DIAG_IOCTL_SWITCH_LOGGING failed with referenced simple command");

        ret = ioctl(fd, DIAG_IOCTL_SWITCH_LOGGING, cur_mode);
        if (ret < 0) {
            LOGD("DIAG_IOCTL_SWITCH_LOGGING failed with direct simple command");

            /* Android 7.0 mode
			 * Reference: https://android.googlesource.com/kernel/msm.git/+/android-7.1.0_r0.3/drivers/char/diag/diagchar_core.c
			 */

            struct diag_logging_mode_param_t new_mode;
            new_mode.req_mode = cur_mode;
            new_mode.peripheral_mask = DIAG_CON_ALL;
            new_mode.mode_param = 0;

            ret = ioctl(fd, DIAG_IOCTL_SWITCH_LOGGING, (char *) &new_mode);
            if (ret < 0) {
                LOGD("Android-7.0 ioctl SWITCH_LOGGING fails: %s \n", strerror(errno));

                ret = ioctl(fd, DIAG_IOCTL_SWITCH_LOGGING, &cur_mode, 12, 0, 0, 0, 0);
                if (ret < 0) {
                    LOGD("S7 Edge fails: %s \n", strerror(errno));
                    return false;
                }
                else{
                    LOGD("DIAG_IOCTL_SWITCH_LOGGING S7 Edge mode OK, response %d\n", ret);
                    return true;
                }
            }else{
                LOGD("DIAG_IOCTL_SWITCH_LOGGING Android 7 mode OK, response %d\n", ret);
                return true;
            }
        }
        else{
            LOGD("DIAG_IOCTL_SWITCH_LOGGING direct simple command OK, response %d\n", ret);
            return true;
        }
    }else{
        LOGD("DIAG_IOCTL_SWITCH_LOGGING referenced simple command OK, response %d\n", ret);
        return true;
    }
}