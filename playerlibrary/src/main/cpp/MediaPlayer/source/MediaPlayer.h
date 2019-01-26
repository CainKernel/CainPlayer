//
// Created by cain on 2018/12/26.
//

#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <sync/MediaClock.h>
#include <SoundTouchWrapper.h>
#include <PlayerState.h>
#include <MediaPlayerCallback.h>
#include <decoder/AudioDecoder.h>
#include <decoder/VideoDecoder.h>

#if defined(__ANDROID__)
#include <device/android/SLESDevice.h>
#include <device/android/GLESDevice.h>
#else
#include <device/AudioDevice.h>
#include <device/VideoDevice.h>
#endif
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <sync/MediaSync.h>
#include <convertor/AudioResampler.h>


class MediaPlayer : public Runnable {
public:
    MediaPlayer();

    virtual ~MediaPlayer();

    void setPlayerCallback(MediaPlayerCallback *playerCallback);

    void setDataSource(const char *url);

    void setVideoDevice(VideoDevice *videoDevice);

    void prepare();

    void start();

    void pause();

    void resume();

    void stop();

    void seekTo(float timeMs);

    void setLooping(int looping);

    void setMute(int mute);

    void setRate(float rate);

    void setPitch(float pitch);

    int getVideoWidth();

    int getVideoHeight();

    int getDuration();

    int isPlaying();

    void pcmQueueCallback(uint8_t *stream, int len);

protected:
    virtual void run();

private:
    int readPackets();

    // prepare decoder with stream_index
    int prepareDecoder(int streamIndex);

    // open an audio output device
    int openAudioDevice(int64_t wanted_channel_layout, int wanted_nb_channels,
                        int wanted_sample_rate);

private:
    Mutex mMutex;
    Condition mCondition;
    Thread *readThread;                     // 读数据包线程

    const char *url;                        // 文件路径
    PlayerState *playerState;               // 播放器状态
    MediaPlayerCallback *playerCallback;    // 播放器回调
    AudioDecoder *audioDecoder;             // 音频解码器
    VideoDecoder *videoDecoder;             // 视频解码器
    bool mExit;

    // 解复用处理
    AVFormatContext *pFormatCtx;            // 解码上下文
    int mDuration;                          // 文件总时长
    int lastPaused;                         // 上一次暂停状态
    int eof;                                // 数据包读到结尾标志
    int attachmentRequest;                  // 视频封面数据包请求

    AudioDevice *audioDevice;               // 音频输出设备
    AudioResampler *audioResampler;         // 音频重采样器

    MediaSync *mediaSync;                   // 媒体同步器

};

#endif //MEDIAPLAYER_H
