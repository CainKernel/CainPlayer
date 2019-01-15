//
// Created by cain on 2018/12/26.
//

#ifndef PLAYEROPTION_H
#define PLAYEROPTION_H

#include <stdio.h>

#include <Mutex.h>
#include <Condition.h>
#include <Thread.h>

#include <common/FFmpegUtils.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mem.h>
#include <libavutil/rational.h>
#include <libavutil/time.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/avstring.h>
};

#define VIDEO_QUEUE_SIZE 3

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25

#define AUDIO_MIN_BUFFER_SIZE 512

#define AUDIO_MAX_CALLBACKS_PER_SEC 30

#define REFRESH_RATE 0.01

#define AV_SYNC_THRESHOLD_MIN 0.04

#define AV_SYNC_THRESHOLD_MAX 0.1

#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1

#define AV_NOSYNC_THRESHOLD 10.0


#define EXTERNAL_CLOCK_MIN_FRAMES 2

#define EXTERNAL_CLOCK_MAX_FRAMES 10

#define EXTERNAL_CLOCK_SPEED_MIN  0.900

#define EXTERNAL_CLOCK_SPEED_MAX  1.010

#define EXTERNAL_CLOCK_SPEED_STEP 0.001



#define AUDIO_DIFF_AVG_NB   20

#define SAMPLE_CORRECTION_PERCENT_MAX 10

typedef enum {
    AV_SYNC_AUDIO,      // 同步到音频时钟
    AV_SYNC_VIDEO,      // 同步到视频时钟
    AV_SYNC_EXTERNAL,   // 同步到外部时钟
} SyncType;

/**
 * 播放器状态结构体
 */
typedef struct PlayerState {

    AVDictionary *sws_dict;         // 视频转码option参数
    AVDictionary *swr_opts;         //
    AVDictionary *format_opts;      // 解复用option参数
    AVDictionary *codec_opts;       // 解码option参数
    AVDictionary *resample_opts;    // 重采样option参数

    const char *audioCodecName;     // 指定音频解码器名称
    const char *videoCodecName;     // 指定视频解码器名称

    int abortRequest;               // 退出标志
    int pauseRequest;               // 暂停标志
    SyncType syncType;              // 同步类型
    int64_t startTime;              // 播放起始位置
    int64_t duration;               // 播放时长
    int realTime;                   // 判断是否实时流
    int infiniteBuffer;             // 是否无限缓冲区，默认为-1
    int audioDisable;               // 是否禁止音频流
    int videoDisable;               // 是否禁止视频流
    int displayDisable;             // 是否禁止显示

    int fast;                       // 解码上下文的AV_CODEC_FLAG2_FAST标志
    int genpts;                     // 解码上下文的AVFMT_FLAG_GENPTS标志
    int lowres;                     // 解码上下文的lowres标志

    float playbackRate;             // 播放速度
    float playbackPitch;            // 播放音调

    int seekByBytes;                // 是否以字节定位
    int seekRequest;                // 定位请求
    int seekFlags;                  // 定位标志
    int64_t seekPos;                // 定位位置
    int64_t seekRel;                // 定位偏移

    int autoExit;                   // 是否自动退出
    int loop;                       // 循环播放
    int mute;                       // 静音播放
    int frameDrop;                  // 舍帧操作

} PlayerState;

/**
 * 重置播放器状态结构体
 * @param state
 */
inline void resetPlayerState(PlayerState *state) {

    av_opt_free(state);

    av_dict_free(&state->sws_dict);
    av_dict_free(&state->swr_opts);
    av_dict_free(&state->format_opts);
    av_dict_free(&state->codec_opts);
    av_dict_free(&state->resample_opts);
    av_dict_set(&state->sws_dict, "flags", "bicubic", 0);

    if (state->audioCodecName != NULL) {
        av_freep(&state->audioCodecName);
        state->audioCodecName = NULL;
    }
    if (state->videoCodecName != NULL) {
        av_freep(&state->videoCodecName);
        state->videoCodecName = NULL;
    }
    state->abortRequest = 1;
    state->pauseRequest = 0;
    state->seekByBytes = 0;
    state->syncType = AV_SYNC_AUDIO;
    state->startTime = AV_NOPTS_VALUE;
    state->duration = AV_NOPTS_VALUE;
    state->realTime = 0;
    state->infiniteBuffer = -1;
    state->audioDisable = 0;
    state->videoDisable = 0;
    state->displayDisable = 0;
    state->fast = 0;
    state->genpts = 0;
    state->lowres = 0;
    state->playbackRate = 1.0;
    state->playbackPitch = 1.0;
    state->seekRequest = 0;
    state->seekFlags = 0;
    state->seekPos = 0;
    state->seekRel = 0;
    state->seekRel = 0;
    state->autoExit = 0;
    state->loop = 0;
    state->frameDrop = 1;
}

#endif //PLAYEROPTION_H
