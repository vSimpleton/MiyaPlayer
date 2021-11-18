package com.miya.player


/**
 * NAME: vSimpleton
 * DATE: 2021/8/30
 * DESC:
 */

class MiyaPlayer {

    companion object {
        init {
            System.loadLibrary("native-lib")
        }

        // 无法打开视频
        private const val FFMPEG_CAN_NOT_OPEN_URL = 1
        // 找不到流媒体
        private const val FFMPEG_CAN_NOT_FIND_STREAMS = 2
        // 找不到解码器
        private const val FFMPEG_FIND_DECODER_FAIL = 3
        // 无法根据解码器创建上下文
        private const val FFMPEG_ALLOC_CODEC_CONTEXT_FAIL = 4
        //  根据流信息 配置上下文参数失败
        private const val FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL = 6
        // 打开解码器失败
        private const val FFMPEG_OPEN_DECODER_FAIL = 7
        // 没有音视频
        private const val FFMPEG_NO_MEDIA = 8
    }

    private var onPreparedListener: OnPreparedListener? = null
    private var onErrorListener: OnErrorListener? = null

    // 媒体源（文件路径或直播地址RTMP）
    var dataSource = ""

    fun prepare() {
        prepareNative(dataSource)
    }

    fun start() {
        startNative()
    }

    fun stop() {
        stopNative()
    }

    fun release() {
        releaseNative()
    }

    /**
     * 提供给JNI调用
     */
    fun onPrepared() {
        onPreparedListener?.onPrepared()
    }

    fun setOnPreparedListener(onPreparedListener: OnPreparedListener) {
        this.onPreparedListener = onPreparedListener
    }

    /**
     * 提供给JNI调用
     */
    fun onError(errorCode: Int) {
        onErrorListener?.let {
            var msg = ""
            when(errorCode) {
                FFMPEG_CAN_NOT_OPEN_URL -> msg = "无法打开视频"
                FFMPEG_CAN_NOT_FIND_STREAMS -> msg = "找不到流媒体"
                FFMPEG_FIND_DECODER_FAIL -> msg = "找不到解码器"
                FFMPEG_ALLOC_CODEC_CONTEXT_FAIL -> msg = "无法根据解码器创建上下文"
                FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL -> msg = "根据流信息 配置上下文参数失败"
                FFMPEG_OPEN_DECODER_FAIL -> msg = "打开解码器失败"
                FFMPEG_NO_MEDIA -> msg = "没有音视频"
            }
            it.onError(msg)
        }
    }

    fun setOnErrorListener(onErrorListener: OnErrorListener) {
        this.onErrorListener = onErrorListener
    }

    private external fun prepareNative(dataSource: String)
    external fun startNative()
    external fun stopNative()
    external fun releaseNative()

}