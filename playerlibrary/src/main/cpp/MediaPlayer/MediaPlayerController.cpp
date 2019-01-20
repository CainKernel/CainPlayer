//
// Created by cain on 2018/11/29.
//

#include <jni.h>
#include "source/MediaPlayer.h"
#include "JniMediaPlayerCallback.h"

_JavaVM *javaVM = NULL;
MediaPlayer *mediaPlayer = NULL;
GLESDevice *videoDevice = NULL;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSetup(JNIEnv *env, jobject instance) {
    if (mediaPlayer == NULL) {
        mediaPlayer = new MediaPlayer();
    }
    mediaPlayer->setPlayerCallback(new JniMediaPlayerCallback(javaVM, env, &instance));
    videoDevice = new GLESDevice();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeRelease(JNIEnv *env, jobject instance) {

    if (mediaPlayer != NULL) {
        mediaPlayer->stop();
        delete mediaPlayer;
        mediaPlayer = NULL;
    }

    if (videoDevice != NULL) {
        delete videoDevice;
        videoDevice = NULL;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSurfaceCreated(JNIEnv *env,
                                                                        jobject instance,
                                                                        jobject surface) {
    if (videoDevice) {
        ANativeWindow *window = NULL;
        if (surface != NULL) {
            window = ANativeWindow_fromSurface(env, surface);
        }
        videoDevice->surfaceCreated(window);
    }
    if (mediaPlayer) {
        mediaPlayer->setVideoDevice(videoDevice);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSurfaceChanged(JNIEnv *env, jobject instance,
                                                                    jint width, jint height) {
    if (videoDevice) {
        videoDevice->surfaceChanged(width, height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSurfaceDestroyed(JNIEnv *env, jobject instance) {

    if (videoDevice) {
        videoDevice->surfaceDestroyed();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSetLooping(JNIEnv *env, jobject instance,
                                                                jboolean looping) {
    if (mediaPlayer != NULL) {
        mediaPlayer->setLooping(looping);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativePrepare(JNIEnv *env, jobject instance,
                                                             jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    if (mediaPlayer != NULL) {
        mediaPlayer->setDataSource(path);
        mediaPlayer->prepare();
    }

    env->ReleaseStringUTFChars(path_, path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeStart(JNIEnv *env, jobject instance) {
    if (mediaPlayer != NULL) {
        mediaPlayer->start();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativePause(JNIEnv *env, jobject instance) {

    if (mediaPlayer != NULL) {
        mediaPlayer->pause();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeResume(JNIEnv *env, jobject instance) {

    if (mediaPlayer != NULL) {
        mediaPlayer->resume();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeStop(JNIEnv *env, jobject instance) {


    if (mediaPlayer != NULL) {
        mediaPlayer->stop();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSeek(JNIEnv *env, jobject instance,
                                                          jfloat timeMs) {

    if (mediaPlayer != NULL) {
        mediaPlayer->seekTo(timeMs);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSetMute(JNIEnv *env, jobject instance,
                                                                     jboolean mute) {

    if (mediaPlayer != NULL) {
        mediaPlayer->setMute(mute);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSetVolume(JNIEnv *env,
                                                                       jobject instance,
                                                                       jint percent) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSetRate(JNIEnv *env, jobject instance,
                                                             jfloat speed) {

    if (mediaPlayer != NULL) {
        mediaPlayer->setRate(speed);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_nativeSetPitch(JNIEnv *env, jobject instance,
                                                              jfloat pitch) {

    if (mediaPlayer != NULL) {
        mediaPlayer->setPitch(pitch);
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_getDuration(JNIEnv *env, jobject instance) {

    if (mediaPlayer != NULL) {
        return mediaPlayer->getDuration();
    }
    return 0;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_isPlaying(JNIEnv *env, jobject instance) {

    if (mediaPlayer != NULL) {
        return mediaPlayer->isPlaying();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_getVideoWidth(JNIEnv *env, jobject instance) {

    if (mediaPlayer != NULL) {
        return mediaPlayer->getVideoWidth();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_cgfay_media_MediaPlayer_AVMediaPlayer_getVideoHeight(JNIEnv *env, jobject instance) {

    if (mediaPlayer != NULL) {
        return mediaPlayer->getVideoHeight();
    }
    return 0;
}