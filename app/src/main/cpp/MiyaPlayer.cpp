#include "MiyaPlayer.h"

#define TAG "Miya"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

MiyaPlayer::MiyaPlayer(const char *data_source, JNICallbackHelper *pHelper) {
    // 使用深拷贝，防止变量被释放后导致悬空指针
    // Java demo.mp4
    // C++ demo.mp4\0
    this->data_source = new char[strlen(data_source) + 1];
    stpcpy(this->data_source, data_source);

    this->helper = pHelper;
}

MiyaPlayer::~MiyaPlayer() {
    if (data_source) {
        delete data_source;
    }
    if (helper) {
        delete helper;
    }
}

void *task_prepare(void *args) {
    auto *player = static_cast<MiyaPlayer *>(args);
    player->_prepare();
    return nullptr;
}

void MiyaPlayer::_prepare() {
    // 第一步，打开媒体播放地址
    context = avformat_alloc_context();
    int ret = avformat_open_input(&context, data_source, nullptr, nullptr);
    if (ret != 0) {
        // 通知上层打开媒体地址失败
        if (helper) {
            helper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
        }
        return;
    }

    // 第二步：查找媒体中的音视频流信息
    ret = avformat_find_stream_info(context, nullptr);
    if (ret < 0) {
        // 通知上层查找音视频流失败
        if (helper) {
            helper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_FIND_STREAMS);
        }
        return;
    }

    // 第三步：循环拿到各个流的信息（音频流、视频流、字幕流）
    for (int i = 0; i < context->nb_streams; i++) {
        // 第四步：获取媒体流
        AVStream *stream = context->streams[i];

        // 第五步：从上面获取的流中获取编解码的参数
        AVCodecParameters *parameters = stream->codecpar;

        // 第六步：根据上面的参数获取解码器
        const AVCodec *codec = avcodec_find_decoder(parameters->codec_id);
        if (!codec) {
            // 通知上层获取解码器失败
            if (helper) {
                helper->onError(THREAD_CHILD, FFMPEG_FIND_DECODER_FAIL);
            }
        }

        // 第七步：获取编解码器的上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(codec);
        if (!codecContext) {
            // 通知上层获取编解码器失败
            if (helper) {
                helper->onError(THREAD_CHILD, FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            }
            return;
        }

        // 第八步：由于第七步得到的codecContext中没有任何内容，需要把上面得到的parameters赋给codecContext
        ret = avcodec_parameters_to_context(codecContext, parameters);
        if (ret < 0) {
            // 通知上层赋值失败
            if (helper) {
                helper->onError(THREAD_CHILD, FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            }
            return;
        }

        // 第九步：打开解码器
        ret = avcodec_open2(codecContext, codec, nullptr);
        if (ret != 0) {
            // 通知上层打开解码器失败
            if (helper) {
                helper->onError(THREAD_CHILD, FFMPEG_OPEN_DECODER_FAIL);
            }
            return;
        }

        // 第十步：从编解码器参数中获取流的类型
        if (parameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_channel = new AudioChannel();
        } else if (parameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_channel = new VideoChannel();
        }
    }

    // 第十一步：校验(流中没有视频也没有音频)
    if (!audio_channel && !video_channel) {
        // 通知上层流中没有音频跟视频
        if (helper) {
            helper->onError(THREAD_CHILD, FFMPEG_NO_MEDIA);
        }
        return;
    }

    // 第十二步：准备成功
    if (helper) {
        helper->onPrepared(THREAD_CHILD);
    }
}

void MiyaPlayer::prepare() {
    pthread_create(&pid_prepare, nullptr, task_prepare, this);
}
