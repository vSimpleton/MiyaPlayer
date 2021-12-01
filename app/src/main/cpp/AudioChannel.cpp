//
// Created by Sherry on 2021/9/1.
//

#include "AudioChannel.h"

AudioChannel::AudioChannel(int stream_index, AVCodecContext *avCodecContext): BaseChannel(stream_index, avCodecContext) {

}

AudioChannel::~AudioChannel() {

}

void AudioChannel::stop() {

}
