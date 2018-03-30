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
#include "consts.h"
#include "diagcfgwr.h"


#define  LOG_TAG    "witnettest"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

volatile bool must_stop = false;

jobject NewInteger(JNIEnv* env, int value);

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
    jboolean retval = true;

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

jobject NewInteger(JNIEnv* env, int value)
{
    jclass integerClass = env->FindClass("java/lang/Integer");
    jmethodID integerConstructor = env->GetMethodID(integerClass, "<init>", "(I)V");
    return env->NewObject(integerClass, integerConstructor, static_cast<jint>(value));
}