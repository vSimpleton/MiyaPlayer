#ifndef MIYAPLAYER_JNICALLBACKHELPER_H
#define MIYAPLAYER_JNICALLBACKHELPER_H

#include <jni.h>
#include "util.h"

class JNICallbackHelper {

private:
    JavaVM *vm = 0;
    JNIEnv *env = 0;
    jobject obj;
    jmethodID mid_prepare;
    jmethodID mid_error;

public:
    JNICallbackHelper(JavaVM *vm, JNIEnv *env, jobject obj);

    ~JNICallbackHelper();

    void onPrepared(int thread_mode);

    void onError(int thread_mode, int error_code);
};


#endif
