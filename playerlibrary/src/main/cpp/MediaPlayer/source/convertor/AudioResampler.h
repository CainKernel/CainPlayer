//
// Created by cain on 2019/1/6.
//

#ifndef AUDIORESAMPLER_H
#define AUDIORESAMPLER_H

#include <PlayerOption.h>
#include <sync/MediaSync.h>
#include <SoundTouchWrapper.h>
#include <device/AudioDevice.h>

/**
 * 音频重采样器
 */
class AudioResampler {
public:
    AudioResampler(PlayerState *playerState, AudioDecoder *audioDecoder, MediaSync *mediaSync);

    virtual ~AudioResampler();

    int setResampleParams(AudioDeviceSpec *spec, int64_t wanted_channel_layout);

    void pcmQueueCallback(uint8_t *stream, int len);

private:
    int audioSynchronize(int nbSamples);

    int audioFrameResample();

private:
    PlayerState *playerState;
    MediaSync *mediaSync;

    AudioDecoder *audioDecoder;             // 音频解码器
    AudioState *audioState;                 // 音频重采样状态
    SoundTouchWrapper *soundTouchWrapper;   // 变速变调处理
};


#endif //AUDIORESAMPLER_H
