package com.cgfay.media.MediaPlayer;

import android.text.TextUtils;
import android.view.Surface;

/**
 * 媒体播放器
 */
public class AVMediaPlayer {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("soundtouch");
        System.loadLibrary("media_player");
    }

    private String mPath;

    private boolean mPrepared;

    // 播放完成回调
    private OnCompletionListener mCompletionListener;
    // 播放出错回调
    private OnErrorListener mErrorListener;
    // 准备完成回调
    private OnPreparedListener mPreparedListener;

    public AVMediaPlayer() {
        nativeSetup();
    }

    @Override
    protected void finalize() throws Throwable {
        nativeRelease();
        super.finalize();
    }

    /**
     * 设置数据源
     * @param path
     */
    public void setDataSource(String path) {
        mPath = path;
    }

    /**
     * 设置是否循环播放
     * @param looping
     */
    public void setLooping(boolean looping) {
        nativeSetLooping(looping);
    }

    /**
     * Surface已创建
     * @param surface
     */
    public void surfaceCreated(Surface surface) {
        nativeSurfaceCreated(surface);
    }

    /**
     * Surface发生变化
     * @param width
     * @param height
     */
    public void surfaceChanged(int width, int height) {
        nativeSurfaceChanged(width, height);
    }

    /**
     * Surface已销毁
     */
    public void surfaceDestroyed() {
        nativeSurfaceDestroyed();
    }

    /**
     * 准备
     */
    public void prepare() {
        if (TextUtils.isEmpty(mPath)) {
            return;
        }
        nativePrepare(mPath);
    }

    /**
     * 开始
     */
    public void start() {
        if (!mPrepared) {
            return;
        }
        nativeStart();
    }

    /**
     * 暂停
     */
    public void pause() {
        nativePause();
    }

    /**
     * 启动
     */
    public void resume() {
        nativeResume();
    }

    /**
     * 停止
     */
    public void stop() {
        nativeStop();
    }

    /**
     * 释放资源
     */
    public void release() {
        nativeRelease();
    }

    /**
     * 定位
     * @param timeMs 毫秒
     */
    public void seekTo(float timeMs) {
        nativeSeek(timeMs);
    }

    /**
     * 设置是否静音
     * @param mute
     */
    public void setMute(boolean mute) {
        nativeSetMute(mute);
    }

    /**
     * 设置音量
     * @param percent
     */
    public void setVolume(int percent) {
        nativeSetVolume(percent);
    }

    /**
     * 设置速度
     * @param rate
     */
    public void setRate(float rate) {
        nativeSetRate(rate);
    }

    /**
     * 设置pitch
     * @param pitch
     */
    public void setPitch(float pitch) {
        nativeSetPitch(pitch);
    }
    /**
     * 是否已经准备
     * @return
     */
    public boolean isPrepared() {
        return mPrepared;
    }

    // native方法
    private native void nativeSetup();
    private native void nativeRelease();
    private native void nativeSurfaceCreated(Surface surface);
    private native void nativeSurfaceChanged(int width, int height);
    private native void nativeSurfaceDestroyed();
    private native void nativeSetLooping(boolean looping);
    private native void nativePrepare(String path);
    private native void nativeStart();
    private native void nativePause();
    private native void nativeResume();
    private native void nativeStop();
    private native void nativeSeek(float timeMs);
    private native void nativeSetMute(boolean mute);
    private native void nativeSetVolume(int percent);
    private native void nativeSetRate(float rate);
    private native void nativeSetPitch(float pitch);

    /**
     * 获取时长
     * @return
     */
    public native int getDuration();

    /**
     * 是否正在播放
     * @return
     */
    public native boolean isPlaying();

    /**
     * 获取视频宽度
     * @return
     */
    public native int getVideoWidth();

    /**
     * 获取视频高度
     * @return
     */
    public native int getVideoHeight();

    /**
     * 准备完成回调，jni层调用
     */
    private void onPrepared() {
        mPrepared = true;
        if (mPreparedListener != null) {
            mPreparedListener.onPrepared();
        }
    }

    /**
     * 出错回调，jni层调用
     * @param code
     * @param msg
     */
    private void onError(int code, String msg) {
        mPrepared = false;
        stop();
        if (mErrorListener != null) {
            mErrorListener.onError(code, msg);
        }
    }

    /**
     * 播放完成回调，jni层调用
     */
    private void onCompletion() {
        mPrepared = false;
        stop();
        if (mCompletionListener != null) {
            mCompletionListener.onCompleted();
        }
    }

    /**
     * 获取音频PCM数据，jni层调用
     * @param bytes
     * @param size
     */
    private void onGetPCM(byte[] bytes, int size) {

    }

    /**
     * 准备完成监听器
     */
    public interface OnPreparedListener {
        void onPrepared();
    }

    /**
     * 设置准备完成回调
     * @param listener
     */
    public void setOnPreparedListener(OnPreparedListener listener) {
        mPreparedListener = listener;
    }

    /**
     * 播放完成监听器
     */
    public interface OnCompletionListener {
        void onCompleted();
    }

    /**
     * 设置完成回调
     * @param listener
     */
    public void setOnCompletionListener(OnCompletionListener listener) {
        mCompletionListener = listener;
    }

    /**
     * 出错监听器
     */
    public interface OnErrorListener {
        void onError(int code, String msg);
    }

    /**
     * 设置出错回调
     * @param listener
     */
    public void setOnErrorListener(OnErrorListener listener) {
        mErrorListener = listener;
    }

}
