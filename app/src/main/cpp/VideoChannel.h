//
// Created by Sherry on 2021/9/1.
// 获取视频流压缩包并解码
//

#ifndef MIYAPLAYER_VIDEOCHANNEL_H
#define MIYAPLAYER_VIDEOCHANNEL_H

#include "BaseChannel.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
}

typedef void(*RenderCallback) (uint8_t *, int, int, int); // 函数指针声明定义

class VideoChannel : public BaseChannel{
private:
    pthread_t pid_video_decode;
    pthread_t pid_video_play;
    RenderCallback renderCallback;

public:
    VideoChannel(int stream_index, AVCodecContext *avCodecContext);

    ~VideoChannel();

    void stop();

    void start();

    void video_decode();

    void video_play();

    void setRenderCallback(RenderCallback renderCallback);
};


#endif //MIYAPLAYER_VIDEOCHANNEL_H
