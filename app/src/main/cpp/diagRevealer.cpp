//
// Created by siralex on 21-Mar-18.
//

#include <jni.h>
#include <android/log.h>


#define  LOG_TAG    "witnettest"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

extern "C"
JNIEXPORT void JNICALL
Java_net_ddns_koshka_witnettest004_DiagRevealerControl_readDiag(
        JNIEnv *env,
        jobject obj) {

    LOGD("DEBUGNET: diagRevealer successfully called\n");

    jclass cls = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(cls, "logRevealer", "()V");
    if (mid == 0)  {
        LOGD("DEBUGNET: unable to find method ID\n");
        return;
    }

    env->CallVoidMethod(obj, mid);
    env->CallVoidMethod(obj, mid);
    return;
}