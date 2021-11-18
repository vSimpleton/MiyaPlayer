#include "JNICallbackHelper.h"

JNICallbackHelper::JNICallbackHelper(JavaVM *vm, JNIEnv *env, jobject obj) {
    this->vm = vm;
    this->env = env;
    this->obj = env->NewGlobalRef(obj);

    jclass clazz = env->GetObjectClass(obj);
    mid_prepare = env->GetMethodID(clazz, "onPrepared", "()V");
    mid_error = env->GetMethodID(clazz, "onError", "(I)V");
}

JNICallbackHelper::~JNICallbackHelper() {
    vm = nullptr;
    env->DeleteGlobalRef(obj);
    env = nullptr;
    obj = nullptr;
}

void JNICallbackHelper::onPrepared(int thread_mode) {
    if (thread_mode == THREAD_MAIN) {
        env->CallVoidMethod(obj, mid_prepare);
    } else {
        JNIEnv *child_env = nullptr;
        vm->AttachCurrentThread(&child_env, nullptr);
        child_env->CallVoidMethod(obj, mid_prepare);
        vm->DetachCurrentThread();
    }
}

void JNICallbackHelper::onError(int thread_mode, int error_code) {
    if (thread_mode == THREAD_MAIN) {
        //主线程
        env->CallVoidMethod(obj, mid_error);
    } else {
        JNIEnv *child_env = nullptr;
        vm->AttachCurrentThread(&child_env, nullptr);
        child_env->CallVoidMethod(obj, mid_error, error_code);
        vm->DetachCurrentThread();
    }
}
