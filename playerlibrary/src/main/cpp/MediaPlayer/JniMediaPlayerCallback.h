//
// Created by cain on 2018/12/9.
//

#ifndef JNIMEDIAPLAYERCALLBACK_H
#define JNIMEDIAPLAYERCALLBACK_H


#include <jni.h>
#include "source/MediaPlayerCallback.h"

class JniMediaPlayerCallback : public MediaPlayerCallback {
public:
    JniMediaPlayerCallback(_JavaVM *javaVM, JNIEnv *env, jobject *obj);

    virtual ~JniMediaPlayerCallback();

    void onPrepared() override;

    void onError(int code, char *msg) override;

    void onComplete() override;

    void onSeekComplete() override;

    void onVideoSizeChanged(int width, int height) override;

    void onGetPCM(uint8_t *pcmData, size_t size) override;

private:
    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;

    jmethodID jmid_prepared;
    jmethodID jmid_error;
    jmethodID jmid_complete;
    jmethodID jmid_seekComplete;
    jmethodID jmid_videoSizeChanged;
    jmethodID jmid_getPCM;
};


#endif //JNIMEDIAPLAYERCALLBACK_H
