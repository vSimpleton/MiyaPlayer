#include <jni.h>
#include <string>
#include "MiyaPlayer.h"
#include "JNICallbackHelper.h"
#include <android/native_window_jni.h>

static JavaVM *javaVm;
const char *className = "com/miya/player/MiyaPlayer";
MiyaPlayer *player = nullptr;
ANativeWindow *nativeWindow = nullptr;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int registerNative(JNIEnv *pEnv);
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

/**
 * ANativeWindow渲染视频
 */
void renderFrame(uint8_t * src_data, int width, int height, int src_lineSize) {
    pthread_mutex_lock(&mutex);

    // 防止出了问题后出现死锁
    if (!nativeWindow) {
        pthread_mutex_unlock(&mutex);
    }
    ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    // 如果在渲染的时候是被锁住的，那么就无法渲染，需要释放，防止出现死锁
    if (ANativeWindow_lock(nativeWindow, &windowBuffer, nullptr)) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = nullptr;

        pthread_mutex_unlock(&mutex);
        return;
    }

    // 开始真正渲染
    auto *dst_data = static_cast<uint8_t *>(windowBuffer.bits);
    int dst_lineSize = windowBuffer.stride * 4;

    for (int i = 0; i < windowBuffer.height; ++i) {
        memcpy(dst_data + i * dst_lineSize, src_data + i * dst_lineSize, dst_lineSize);
    }

    // 解锁并刷新
    ANativeWindow_unlockAndPost(nativeWindow);

    pthread_mutex_unlock(&mutex);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_miya_player_MiyaPlayer_prepareNative(JNIEnv *env, jobject job, jstring data_source) {
    const char * data_source_ = env->GetStringUTFChars(data_source, nullptr);
    auto *helper = new JNICallbackHelper(javaVm, env, job);
    player = new MiyaPlayer(data_source_, helper);
    player->setRenderCallback(renderFrame);
    player->prepare();
    env->ReleaseStringUTFChars(data_source, data_source_);
}

void startNative() {
    if (player) {
        player->start();
    }
}

void stopNative() {

}

void releaseNative() {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_miya_player_MiyaPlayer_setSurfaceNative(JNIEnv *env, jobject thiz, jobject surface) {
    pthread_mutex_lock(&mutex);

    // 先释放之前的显示窗口
    if (nativeWindow) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = nullptr;
    }
    nativeWindow = ANativeWindow_fromSurface(env, surface);

    pthread_mutex_unlock(&mutex);
}