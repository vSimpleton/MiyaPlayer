#include <jni.h>
#include <string>
#include "MiyaPlayer.h"
#include "JNICallbackHelper.h"

static JavaVM *javaVm;
const char *className = "com/miya/player/MiyaPlayer";
MiyaPlayer *player = nullptr;

int registerNative(JNIEnv *pEnv);
//void prepareNative(jstring dataSource);
void startNative();
void stopNative();
void releaseNative();

const JNINativeMethod jniNativeMethod[] = {
//        {"prepareNative", "(Ljava/lang/String;)V", (void *) prepareNative},
        {"startNative", "()V", (void *) startNative},
        {"stopNative", "()V", (void *) stopNative},
        {"releaseNative", "()V", (void *) releaseNative},
};

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    ::javaVm = vm;
    JNIEnv *env = nullptr;
    int result = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (result != JNI_OK) {
        return -1;
    }

    registerNative(env);
    return JNI_VERSION_1_6;
}

int registerNative(JNIEnv *pEnv) {
    jclass clazz = pEnv->FindClass(className);
    if (clazz == nullptr) {
        return JNI_FALSE;
    }

    int result = pEnv->RegisterNatives(clazz, jniNativeMethod, sizeof(jniNativeMethod) / sizeof(JNINativeMethod));
    if (result < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

//void prepareNative(jstring dataSource) {
//    JNIEnv *env = nullptr;
//    int result = javaVm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
//    if (result != JNI_OK) {
//        return;
//    }
//
//    jclass clazz = env->FindClass(className);
//    jmethodID mid = env->GetMethodID(clazz, "prepareNative", "(Ljava/lang/String;)V");
//    jobject obj = env->NewObject(clazz, mid);
//
//    const char *data_source = env->GetStringUTFChars(dataSource, nullptr);
//    auto *helper = new JNICallbackHelper(javaVm, env, obj);
//    auto *player = new MiyaPlayer(data_source, helper);
//    player->prepare();
//    env->ReleaseStringUTFChars(dataSource, data_source);
//}

extern "C"
JNIEXPORT void JNICALL
Java_com_miya_player_MiyaPlayer_prepareNative(JNIEnv *env, jobject job, jstring data_source) {
    const char * data_source_ = env->GetStringUTFChars(data_source, nullptr);
    auto *helper = new JNICallbackHelper(javaVm, env, job);
    player = new MiyaPlayer(data_source_, helper);
    player->prepare();
    env->ReleaseStringUTFChars(data_source, data_source_);
}

void startNative() {
    if (player) {
//        player.start();
    }
}

void stopNative() {

}

void releaseNative() {

}