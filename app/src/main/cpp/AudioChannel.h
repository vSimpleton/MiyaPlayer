//
// Created by Sherry on 2021/9/1.
// 获取音频流压缩包并解码
//

#ifndef MIYAPLAYER_AUDIOCHANNEL_H
#define MIYAPLAYER_AUDIOCHANNEL_H

#include "BaseChannel.h"

class AudioChannel : public BaseChannel{

public:
    AudioChannel(int stream_index, AVCodecContext *avCodecContext);

    ~AudioChannel();

    void stop();
};


#endif //MIYAPLAYER_AUDIOCHANNEL_H
