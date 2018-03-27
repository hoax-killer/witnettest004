//
// Created by siralex on 21-Mar-18.
//

#include <jni.h>
#include <android/log.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define  LOG_TAG    "witnettest"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

volatile bool must_stop = false;

extern "C"
JNIEXPORT void JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_readDiag(
        JNIEnv *env,
        jobject obj) {
    LOGD("DEBUGNET: diagRevealer successfully called\n");
    must_stop = false;

    char len = 5;
    char* data = (char *) malloc( len );  //{8,53,116,218,42};

    if (data == NULL) {
        LOGD("DEBUGNET: malloc failed\n");
        return;
    }

    data[0] = 8;
    data[1] = 53;
    data[2] = 116;
    data[3] = 218;
    data[4] = 42;

    jclass cls = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(cls, "logRevealer", "([B)V");
    if (mid == 0) {
        LOGD("DEBUGNET: unable to find method ID\n");
        return;
    }

    jbyteArray bArray = env->NewByteArray(len);
    env->SetByteArrayRegion(bArray, 0, len, (const jbyte *) data);
    while(1) {
        env->CallVoidMethod(obj, mid, bArray);

        usleep(1000000);
        if(must_stop){
            must_stop = false;
            break;
        }
    }
    //env->CallVoidMethod(obj, mid);
    //env->CallVoidMethod(obj, mid);
    return;
}

extern "C"
JNIEXPORT void JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_stopDiag(
        JNIEnv *env,
        jobject obj) {
    if(!must_stop) must_stop = true;
}