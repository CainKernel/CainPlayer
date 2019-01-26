//
// Created by cain on 2018/12/17.
//

#ifndef MEDIAPLAYERCALLBACK_H
#define MEDIAPLAYERCALLBACK_H

#include <stdio.h>

class MediaPlayerCallback {
public:
    MediaPlayerCallback();

    virtual ~MediaPlayerCallback();

    // 准备完成回调
    virtual void onPrepared();

    // 播放出错
    virtual void onError(int code, char *msg);

    // 播放完成
    virtual void onComplete();

    // 定位完成回调
    virtual void onSeekComplete();

    // 视频宽高发生变化
    virtual void onVideoSizeChanged(int width, int height);

    // 音频PCM数据
    virtual void onGetPCM(uint8_t *pcmData, size_t size);
};


#endif //MEDIAPLAYERCALLBACK_H
