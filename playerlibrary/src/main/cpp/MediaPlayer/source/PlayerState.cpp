//
// Created by cain on 2019/1/26.
//

#include "PlayerState.h"

PlayerState::PlayerState() {
    init();
    reset();
}

PlayerState::~PlayerState() {
    reset();
}

void PlayerState::init() {
    sws_dict = (AVDictionary *) malloc(sizeof(AVDictionary));
    memset(sws_dict, 0, sizeof(AVDictionary));
    swr_opts = (AVDictionary *) malloc(sizeof(AVDictionary));
    memset(swr_opts, 0, sizeof(AVDictionary));
    format_opts = (AVDictionary *) malloc(sizeof(AVDictionary));
    memset(format_opts, 0, sizeof(AVDictionary));
    codec_opts = (AVDictionary *) malloc(sizeof(AVDictionary));
    memset(codec_opts, 0, sizeof(AVDictionary));
    resample_opts = (AVDictionary *) malloc(sizeof(AVDictionary));
    memset(resample_opts, 0, sizeof(AVDictionary));
    audioCodecName = NULL;
    videoCodecName = NULL;
}

void PlayerState::reset() {

    if (sws_dict) {
        av_dict_free(&sws_dict);
        av_dict_set(&sws_dict, "flags", "bicubic", 0);
    }
    if (swr_opts) {
        av_dict_free(&swr_opts);
    }
    if (format_opts) {
        av_dict_free(&format_opts);
    }
    if (codec_opts) {
    av_dict_free(&codec_opts);
    }

    if (resample_opts) {
        av_dict_free(&resample_opts);
    }

    if (audioCodecName != NULL) {
        av_freep(&audioCodecName);
        audioCodecName = NULL;
    }
    if (videoCodecName != NULL) {
        av_freep(&videoCodecName);
        videoCodecName = NULL;
    }
    abortRequest = 1;
    pauseRequest = 0;
    seekByBytes = 0;
    syncType = AV_SYNC_AUDIO;
    startTime = AV_NOPTS_VALUE;
    duration = AV_NOPTS_VALUE;
    realTime = 0;
    infiniteBuffer = -1;
    audioDisable = 0;
    videoDisable = 0;
    displayDisable = 0;
    fast = 0;
    genpts = 0;
    lowres = 0;
    playbackRate = 1.0;
    playbackPitch = 1.0;
    seekRequest = 0;
    seekFlags = 0;
    seekPos = 0;
    seekRel = 0;
    seekRel = 0;
    autoExit = 0;
    loop = 0;
    frameDrop = 1;
}
