//
// Created by Sherry on 2021/11/19.
//

#ifndef MIYAPLAYER_BASECHANNEL_H
#define MIYAPLAYER_BASECHANNEL_H

#include "safe_queue.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class BaseChannel {

public:
    int stream_index; // 音频流或视频流的下标
    SafeQueue<AVPacket *> packets; // 压缩数据包
    SafeQueue<AVFrame *> frames; // 压缩数据包解码后的原始数据包
    bool isPlaying;
    AVCodecContext *avCodecContext = nullptr; // 解码器上下文

    BaseChannel(int stream_index, AVCodecContext *avCodecContext): stream_index(stream_index), avCodecContext(avCodecContext) {
        packets.setReleaseCallback(releaseAVPacket);
        frames.setReleaseCallback(releaseAVFrame);
    }

    virtual ~BaseChannel() {
        packets.clear();
        frames.clear();
    }

    /**
     * 释放队列中的所有AVPacket *
     * @param p
     */
    static void releaseAVPacket(AVPacket **p) {
        if (p) {
            av_packet_free(p);
            *p = 0;
        }
    }

    /**
     * 释放队列中的所有AVFrame *
     * @param f
     */
    static void releaseAVFrame(AVFrame **f) {
        if (f) {
            av_frame_free(f);
            *f = 0;
        }
    }

};

#endif //MIYAPLAYER_BASECHANNEL_H
