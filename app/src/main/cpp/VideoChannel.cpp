//
// Created by Sherry on 2021/9/1.
//

#include "VideoChannel.h"

VideoChannel::VideoChannel(int stream_index, AVCodecContext *avCodecContext): BaseChannel(stream_index, avCodecContext) {

}

VideoChannel::~VideoChannel() {

}

void VideoChannel::stop() {

}

void *thread_video_decode(void *args) {
    auto *video_channel = static_cast<VideoChannel *>(args);
    video_channel->video_decode();
    return nullptr;
}

void *task_video_play(void *args) {
    auto *video_channel = static_cast<VideoChannel *>(args);
    video_channel->video_play();
    return nullptr;
}

void VideoChannel::start() {
    isPlaying = true;
    packets.setWorkStatus(1);
    frames.setWorkStatus(1);

    // 第一个线程：取出队列中的压缩数据包进行解码，解码后的原始包push到队列中
    pthread_create(&pid_video_decode, nullptr, thread_video_decode, this);
    // 第二个线程：取出队列中的原始包，进行播放
    pthread_create(&pid_video_play, nullptr, task_video_play, this);
}

/**
 * 解码
 */
void VideoChannel::video_decode() {
    AVPacket *packet = nullptr;
    while (isPlaying) {
        int ret = packets.popToQueue(packet);
        if (!isPlaying) {
            break;
        }

        if (!ret) {
            continue;
        }

        /**
         * 最新版本的FFmpeg，和旧版本差别很大，
         * 新版本：
         * 1.发送pkt（压缩包）给缓冲区，
         * 2.从缓冲区拿出来（原始包）
         */
        ret = avcodec_send_packet(avCodecContext, packet);
        // FFmpeg在调用avcodec_send_packet的时候会缓存一份packet，所以这里可以直接释放掉
        releaseAVPacket(&packet);
        if (ret) {
            break;
        }

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, frame);
        // output is not available in this state - user must try to send new input
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret) {
            break;
        }
        frames.insertToQueue(frame);
    }
    releaseAVPacket(&packet);
}

/**
 * 播放
 */
void VideoChannel::video_play() {
    AVFrame *frame = nullptr;

    uint8_t *dst_data[4];
    int dst_lineSize[4];

    av_image_alloc(dst_data, dst_lineSize, avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA, 1);

    SwsContext *swsContext = sws_getContext(
            // 输入
            avCodecContext->width,
            avCodecContext->height,
            avCodecContext->pix_fmt, // 自动获取像素格式，也可以直接填入 AV_PIX_FMT_YUV420P

            // 输出
            avCodecContext->width,
            avCodecContext->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    while (isPlaying) {
        int ret = frames.popToQueue(frame);
        if (!isPlaying) {
            break;
        }

        if (!ret) {
            continue;
        }

        // 格式转换，YUV -> RGBA
        sws_scale(swsContext, frame->data, frame->linesize, 0, avCodecContext->height,
                  dst_data, dst_lineSize);

        // ANativeWindows 渲染
        // 拿不到SurfaceView，回调到native-lib.cpp
        renderCallback(dst_data[0], avCodecContext->width, avCodecContext->height, dst_lineSize[0]);
        releaseAVFrame(&frame);
    }

    releaseAVFrame(&frame);
    isPlaying = false;
    av_free(&dst_data[0]);
    sws_freeContext(swsContext);
}

void VideoChannel::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}
