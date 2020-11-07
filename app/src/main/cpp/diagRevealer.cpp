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
#include "ws_wrap.h"
#include <poll.h>
#include <thread>
#include <pthread.h>
#include <atomic>


//TODO логирование во всех сишных файлах инклюдить через один общий файл - utils кажется
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




std::atomic<bool> worker_thread_finished (false);
volatile int worker_thread_result = -1;
volatile bool epan_initialized = false;

void handle_diag_write_interrupt(int sig)
{
    // this interrupt is called if writing to diag port hangs
    LOGD("caught signal: %d \n", sig);
    pthread_exit(0);
}

jobject NewInteger(JNIEnv* env, int value);
bool _diag_switch_logging(int fd, int log_mode);
void _gather_diag_data(JavaVM* jvm, jobject obj, int fd);
std::string decode_msg(std::string msgType, std::string msgData);


static unsigned long long
get_posix_timestamp () {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long mslong = (unsigned long long) tv.tv_sec * 1000L + tv.tv_usec / 1000;
    //unsigned long long mslong = 1468496250207L;
    return  mslong;

}

extern "C"
JNIEXPORT jstring JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_wsTest(
        JNIEnv *env,
        jobject obj,
        jbyte testnum){

    LOGD("test procedure is called\n");
    if(!epan_initialized) {
        LOGD("preparing to execute initialize_epan()\n");
        initialize_epan();
        epan_initialized = true;
        LOGD("epan init executed\n");
    }else{
        LOGD("epan already initialized\n");
    }

    std::string resultstr;
    switch((unsigned char)testnum){
        case 1:
            resultstr = "Wireshark decoding test #1\n\"LTE-RRC_PCCH\", \"4001BF281AEBA00000\"\n";
            resultstr.append(decode_msg("LTE-RRC_PCCH", "4001BF281AEBA00000"));
            break;
        case 2:
            resultstr = "Wireshark decoding test #2\n\"RRC_MIB\", \"60c428205aa2fe0090c8506e422419822a3653940c40c0\"\n";
            resultstr.append(decode_msg("RRC_MIB", "60c428205aa2fe0090c8506e422419822a3653940c40c0"));
            break;
        case 3:
            resultstr = "Wireshark decoding test #3\n\"RRC_MIB\", \"10c424c05aa2fe00a0c850448c466608a8e54a80100a0100\"\n";
            resultstr.append(decode_msg("RRC_MIB", "10c424c05aa2fe00a0c850448c466608a8e54a80100a0100"));
            break;
        case 4:
            resultstr = "Wireshark decoding test #4\n\"RRC_SIB1\", \"c764b108500b1ba01483078a2be62ad0\"\n";
            resultstr.append(decode_msg("RRC_SIB1", "c764b108500b1ba01483078a2be62ad0"));
            break;
        case 5:
            resultstr = "Wireshark decoding test #5\n\"RRC_SIB3\", \"0d801f4544fc60005001000011094e\"\n";
            resultstr.append(decode_msg("RRC_SIB3", "0d801f4544fc60005001000011094e"));
            break;
        case 6:
            resultstr = "Wireshark decoding test #6\n\"RRC_SIB5\", \"63403AFFFF03FFFC5010F0290C0A8018000C8BF5B15EA0000003F5210E30000247894201400010440060222E56300C60202C000C14CC003C4300B6D830021844A0585760186AF400\"\n";
            resultstr.append(decode_msg("RRC_SIB5", "63403AFFFF03FFFC5010F0290C0A8018000C8BF5B15EA0000003F5210E30000247894201400010440060222E56300C60202C000C14CC003C4300B6D830021844A0585760186AF400"));
            break;
        case 7:
            resultstr = "Wireshark decoding test #7\n\"RRC_SIB7\", \"018000\"\n";
            resultstr.append(decode_msg("RRC_SIB7", "018000"));
            break;
        case 8:
            resultstr = "Wireshark decoding test #8\n\"RRC_SIB12\", \"b38111d024541a42a0\"\n";
            resultstr.append(decode_msg("RRC_SIB12", "b38111d024541a42a0"));
            break;
        case 9:
            resultstr = "Wireshark decoding test #9\n\"RRC_SIB19\", \"41a1001694e49470\"\n";
            resultstr.append(decode_msg("RRC_SIB19", "41a1001694e49470"));
            break;
        default:
            resultstr = "Wireshark decoding test unsupported\n";

    }
    //std::string mystr =
    //std::string mystr =  decode_msg("RRC_MIB", "60c428205aa2fe0090c8506e422419822a3653940c40c0");
    //LOGD("--- %s",mystr.c_str());
    jstring result = env->NewStringUTF(resultstr.c_str());
    return result;
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

/*
 * function gets a chunk of raw data from DIAG and returns array of decoded messages
 */
extern "C"
JNIEXPORT jobjectArray JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_processLogChunk(
        JNIEnv *env,
        jobject obj,
        jbyteArray pktbytes){

    char* payload_buf = (char*)env->GetByteArrayElements( pktbytes, NULL);
    int buf_length = env->GetArrayLength(pktbytes);

    vector<string> out_json_data = {};
// we don't know how many network messages can be passed within one DIAG message
// we also don't know that the boundary of DIAG message is valid HDLC boundary
// that is why we push all DIAG messages is one buffer and extract network packets from there
// as HDLC frames using HDLC boundaries
    feed_binary(payload_buf, buf_length);

    string frame = "";
    bool crc_correct    = false;

    while(get_next_frame(frame, crc_correct)){
        if ( !crc_correct ){
            LOGD("wrong HDLC frame CRC\n");
            continue;
        }

        if(is_log_packet(frame.data(), frame.size())){

            const char *s = frame.data();
// https://android.googlesource.com/kernel/msm.git/+/android-7.1.0_r0.3/drivers/char/diag/diag_dci.c
// функция extract_dci_log
// 8 = 2 (0x1000) + 2 (len1) + 2 (log_msg_len) + 2 (type_id)
            string json_packet = decode_log_packet(s + 2,  frame.size() - 2, false);
            out_json_data.push_back(json_packet);
        }else{
            LOGD("NOT LOG PACKET\n");
        }

    }

    //LOGD("json size to send: %i\n", out_json_data.size());
    // prepare array of objects to return gathered data
    jobjectArray ret= (jobjectArray)env->NewObjectArray(out_json_data.size(), env->FindClass("java/lang/Object"),NULL);
    for(int i=0; i<out_json_data.size(); i++){
        env->SetObjectArrayElement(ret,i, env->NewStringUTF(out_json_data[i].c_str()));
    }


    env->ReleaseByteArrayElements(pktbytes, (jbyte*)payload_buf, 0);
    return ret;
}



extern "C"
JNIEXPORT jobjectArray JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_getTypeNames(
        JNIEnv *env,
        jobject obj ){

    jobjectArray ret;

    int cnt = 0;
    for(int i=0; i<LogPacketTypeID_To_Name.size(); i++){if(LogPacketTypeID_To_Name[i].enabled) cnt++;}
    ret= (jobjectArray)env->NewObjectArray(cnt, env->FindClass("java/lang/Object"),NULL);


/*
    for(int i=0; i<LogPacketTypeID_To_Name.size(); i++)
        env->SetObjectArrayElement(ret,i,env->NewStringUTF(LogPacketTypeID_To_Name[i].name));
*/
    int j=0;
    for(int i=0; i<LogPacketTypeID_To_Name.size(); i++){
        if(!LogPacketTypeID_To_Name[i].enabled) continue;

        jobjectArray retobjarr= (jobjectArray)env->NewObjectArray(2,env->FindClass("java/lang/Object"),NULL);
        env->SetObjectArrayElement(retobjarr,0,env->NewStringUTF(LogPacketTypeID_To_Name[i].name));
        env->SetObjectArrayElement(retobjarr,1,NewInteger(env, LogPacketTypeID_To_Name[i].val));

        env->SetObjectArrayElement(ret,j++,retobjarr);
    }
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
// this function is executed in separate thread because reading from diag blocking
static int
write_commands (int fd, binaryBuffer *pbuf_write)
{
    worker_thread_finished = false;
    worker_thread_result = -1;
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
        worker_thread_finished = true;
        return -1;
    }

    // prepare signal handler

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = handle_diag_write_interrupt;
    sigset_t   set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2); // SIGUSR1 is blocked in Android
    act.sa_mask = set;
    sigaction(SIGUSR2, &act, 0);

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
                worker_thread_finished = true;
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
                worker_thread_finished = true;
                return -1;
            } else {
                LOGD("Reading %d bytes of resp\n", read_len);
            }
        }
        i += len;
    }
    LOGD("write_commands: OK, exiting\n");
    worker_thread_result = 0; // success
    fflush(stdout);
    worker_thread_finished = true;

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

    // we are launching this task in separate thread because, at least on my phone, the process
    // of reading from DIAG interface is blocking. All attempts to set this device to non blocking mode were
    // failed along with attempts to interrupt the process of reading which always resume after interrupt
    // regardless of SA_RESTART flag. poll and select are also unable to prevent blocking.
    std::thread t1(write_commands, fd, &buf_write);
    pthread_t t1h = t1.native_handle();

    worker_thread_finished = false;
    // give 30sec to write config to the port
    unsigned int retries_n = 60;
    unsigned int retry_t = 500; //500ms in microseconds * 60 attempts
    // wait for timeout or returning from working thread (successful or not)
    while(retries_n > 0){
        if(worker_thread_finished) break;
        usleep(retry_t);
        retries_n--;
    }

    if(worker_thread_result != 0){// result unsuccessful
        LOGD("worker_thread_result != 0");
        if(retries_n <= 0) { // timeout (thread hangs), killing working thread
            LOGD("worker_thread: timeout detected\n");
            t1.detach();
            int status;
            if ( (status = pthread_kill(t1h, SIGUSR2)) != 0){
                LOGD("Error cancelling thread %d, error = %d (%s)", t1h, status, strerror(status));
            }
        }else{
            LOGD("worker_thread: no timeout\n");
            t1.join();
        }

        LOGD("thread_result_2: %i\n", worker_thread_result);
        myresult <<  "Writing Diag.cfg to the port failed. \n" << "unable to continue\n";
        LOGD("Write diag config failed\n");
        env->SetObjectArrayElement(retobjarr,0,NewInteger(env, -1));
        env->SetObjectArrayElement(retobjarr,1,env->NewStringUTF(myresult.str().c_str()));
        close(fd);
        return retobjarr;
    }
    t1.join();

    LOGD("Write diag config OK\n");

    fflush(stdout);
    free(buf_write.p);
    reset_binary(); // resetting buffer for HDLC decoding

    must_stop = false;
    worker_thread_finished = false;
    worker_thread_result = -1;
    JavaVM* jvm;
    env->GetJavaVM(&jvm);
    jobject myobj = env->NewGlobalRef(obj);

    // reading data from diag in separate thread because the reading is blocking
    // and we can't properly stop reading if there is no packets
    std::thread t2(_gather_diag_data, jvm, myobj, fd);
    pthread_t t2h = t2.native_handle();
    while(1){ // wait for command to stop
        if(must_stop)break;
        usleep(50000);
    }

    myresult <<  " \nStop command received.";

    // wait for thread to notice the stop command (5 sec)
    retries_n = 100;
    retry_t = 50000; //50ms
    // wait for timeout or returning from working thread (successful or not)
    while(retries_n > 0){
        if(worker_thread_finished) break;
        usleep(retry_t);
        retries_n--;
    }

    if(!worker_thread_finished){ //timeout
        // because we gave 5 seconds to the thread to notice must_stop variable
        // we presume that it is not inside Java call. 5 sec is enough to exit java call
        // that is why we kill the thread without any mutex to avoid killing during access to Java
        LOGD("worker_thread: timeout detected\n");
        myresult <<  " \nDiag logging killed by timeout.";
        t2.detach();
        int status;
        if ( (status = pthread_kill(t2h, SIGUSR2)) != 0){
            LOGD("Error cancelling thread %d, error = %d (%s)", t2h, status, strerror(status));
        }
    }else{
        myresult <<  " \nDiag logging stopped gracefully.";
        LOGD("worker_thread: no timeout\n");
        t2.join();
    }
    //t2.~thread();
    env->DeleteGlobalRef(myobj);
    //TODO как бы останавливать логирование? Может формировать и записывать пустой diag.cfg?
    // через ioctl вызывть функц. diag_state_close_smd() или из memory_device_mode в no_logging_mode?
/*
    LOGD("Trying to stop logs\n");
    char  tmpbuf[128];
    // two commands: DISABLE_DEBUG and DISABLE
    for(int comm_id = 1; comm_id < 2; comm_id ++){
        string* framep = disable_logs_command( comm_id);
        if( framep == NULL){
            LOGD("Failed to stop  logging. Encoding failed.\n");
            //myresult <<  " \nFailed to stop  logging. Encoding failed.";
            break;
        }

        string frame = *framep;

        *((int *)tmpbuf) = htole32(USER_SPACE_DATA_TYPE); // header
        memcpy(tmpbuf+4, frame.data(),frame.size() ); // command
        ret = write (fd,  tmpbuf, frame.size()+4);

        if(ret<0) {
            LOGD("Stop logging error (err=%d): %s\n", ret, strerror(errno));
            //myresult <<  " \nFailed to stop logging!";
            break;
        }
        if((ret >= 0) && (comm_id == 1)){
            LOGD("Stop logging commands succeeded\n");
            //myresult <<  " \nStop logging commands succeed.";
        }

    }


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


void _gather_diag_data(JavaVM* jvm, jobject obj, int fd){
    int ret;
    JNIEnv * env;
    jclass cls;
    jmethodID mid;

    // attaching to the main thread to get method from object-caller
    if (jvm->AttachCurrentThread(&env, NULL) != 0) {
        LOGD("diag_gatherer: failed to attach to main thread\n");
        worker_thread_finished = true;
        return;
    }else {
        cls = env->GetObjectClass(obj);
        if(cls == NULL){
            LOGD("diag_gatherer: unable to get class!\n");
            worker_thread_finished = true;
            return;
        }
        mid = env->GetMethodID(cls, "logRevealer", "([B[B)V");
        if(mid == NULL){
            LOGD("diag_gatherer: unable to get callback function!\n");
            worker_thread_finished = true;
            return;
        }
        jvm->DetachCurrentThread();
    }



    LOGD("trying to read logs\n");


    // in case of device that has non-blocking Diag
    struct pollfd  fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;

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
                    // first byte is a global packet type - log packet o modem debug message/
                    // log packet should be 10 00, modem debug can vary
                    // then goes message size two times (2b + 2b)
                    // then type_id
                    LOGD("pkt-1: %02X %02X | %02X %02X | %02X %02X |%02X %02X |\n",buf_send[0],buf_send[1],buf_send[2],buf_send[3],buf_send[4],buf_send[5],buf_send[6],buf_send[7]);
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

                    // attaching to the main thread each time we need to access JVM
                    if (jvm->AttachCurrentThread(&env, NULL) != 0) {
                        LOGD("log reader: failed to attach to main thread\n");
                    }else {
                        jbyteArray headerArray = env->NewByteArray(head_offset);
                        env->SetByteArrayRegion(headerArray, 0, head_offset, (const jbyte *) buf_head);

                        jbyteArray dataArray = env->NewByteArray(send_offset);
                        env->SetByteArrayRegion(dataArray, 0, send_offset, (const jbyte *) buf_send);

                        env->CallVoidMethod(obj, mid, headerArray, dataArray);

                        env->DeleteLocalRef(headerArray);
                        env->DeleteLocalRef(dataArray);

                        jvm->DetachCurrentThread();
                    }


//                    jbyteArray bArray = env->NewByteArray(send_offset);
//                    env->SetByteArrayRegion(bArray, 0, send_offset, (const jbyte *) buf_send);
//                    env->CallVoidMethod(obj, mid, bArray);

//                    env->DeleteLocalRef(bArray);


                    //LOGD("packet been sent to FIFO. size: %i\n", send_offset);

                }else{
                    // TODO: Check other raw binary types
                    // LOGI("Not USER_SPACE_DATA_TYPE: %d\n", *((int *)buf_read));
                }
            }
        }

        if(must_stop){
            LOGD("diag_gatherer detected stop command\n");
            break;
        }

    }

    worker_thread_finished = true;
    return;
}