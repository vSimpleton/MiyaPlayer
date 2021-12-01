#ifndef MIYAPLAYER_MIYAPLAYER_H
#define MIYAPLAYER_MIYAPLAYER_H

#include <cstring>
#include <pthread.h>
#include "AudioChannel.h"
#include "VideoChannel.h"
#include "JNICallbackHelper.h"
#include "util.h"
#include <android/log.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class MiyaPlayer {

private:
    char *data_source = 0;
    pthread_t pid_prepare;
    pthread_t pid_start;
    AVFormatContext *context = 0; // 媒体上下文
    AudioChannel *audio_channel = 0;
    VideoChannel *video_channel = 0;
    JNICallbackHelper *helper = 0;
    bool isPlaying = false; // 是否正在播放

    RenderCallback renderCallback;

public:
    MiyaPlayer(const char *data_source, JNICallbackHelper *pHelper);

    ~MiyaPlayer();

    void prepare();

    void _prepare();

    void start();

    void _start();

    void setRenderCallback(RenderCallback renderCallback);
};


#endif
