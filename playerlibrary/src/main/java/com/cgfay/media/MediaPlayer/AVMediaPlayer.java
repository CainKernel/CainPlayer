package com.cgfay.media.MediaPlayer;

import android.content.Context;
import android.os.PowerManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

/**
 * 媒体播放器
 */
public class AVMediaPlayer {

    private static final String TAG = "AVMediaPlayer";

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("soundtouch");
        System.loadLibrary("media_player");
    }

    // 设置SurfaceHolder
    private SurfaceHolder mSurfaceHolder;
    // 电源管理锁
    private PowerManager.WakeLock mWakeLock = null;
    // 播放阶段屏幕常亮
    private boolean mScreenOnWhilePlaying;
    // 是否保持常亮
    private boolean mStayAwake;
    // 路径
    private String mPath;
    // 准备完成标志
    private boolean mPrepared;
    // 定位标志
    private boolean mSeeking;

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
     * 设置显示
     * @param sh
     */
    public void setDisplay(SurfaceHolder sh) {
        mSurfaceHolder = sh;
        Surface surface;
        if (sh != null) {
            surface = sh.getSurface();
        } else {
            surface = null;
        }
        nativeSurfaceCreated(surface);
        updateSurfaceScreenOn();
    }

    /**
     * 设置SurfaceSurface
     * @param surface
     */
    public void setSurface(Surface surface) {
        if (mScreenOnWhilePlaying && surface != null) {
            Log.w(TAG, "setScreenOnWhilePlaying(true) is ineffective for Surface");
        }
        mSurfaceHolder = null;
        nativeSurfaceCreated(surface);
        updateSurfaceScreenOn();
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
        if (mSeeking) {
            return;
        }
        mSeeking = true;
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
     * 设置电源模式
     * @param context
     * @param mode
     */
    public void setWakeMode(Context context, int mode) {
        boolean washeld = false;

        if (mWakeLock != null) {
            if (mWakeLock.isHeld()) {
                washeld = true;
                mWakeLock.release();
            }
            mWakeLock = null;
        }

        PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(mode|PowerManager.ON_AFTER_RELEASE, AVMediaPlayer.class.getName());
        mWakeLock.setReferenceCounted(false);
        if (washeld) {
            mWakeLock.acquire();
        }
    }

    /**
     * Control whether we should use the attached SurfaceHolder to keep the
     * screen on while video playback is occurring.  This is the preferred
     * method over {@link #setWakeMode} where possible, since it doesn't
     * require that the application have permission for low-level wake lock
     * access.
     *
     * @param screenOn Supply true to keep the screen on, false to allow it
     * to turn off.
     */
    public void setScreenOnWhilePlaying(boolean screenOn) {
        if (mScreenOnWhilePlaying != screenOn) {
            if (screenOn && mSurfaceHolder == null) {
                Log.w(TAG, "setScreenOnWhilePlaying(true) is ineffective without a SurfaceHolder");
            }
            mScreenOnWhilePlaying = screenOn;
            updateSurfaceScreenOn();
        }
    }

    private void stayAwake(boolean awake) {
        if (mWakeLock != null) {
            if (awake && !mWakeLock.isHeld()) {
                mWakeLock.acquire();
            } else if (!awake && mWakeLock.isHeld()) {
                mWakeLock.release();
            }
        }
        mStayAwake = awake;
        updateSurfaceScreenOn();
    }

    private void updateSurfaceScreenOn() {
        if (mSurfaceHolder != null) {
            mSurfaceHolder.setKeepScreenOn(mScreenOnWhilePlaying && mStayAwake);
        }
    }

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
        if (mErrorListener != null) {
            mErrorListener.onError(code, msg);
        }
    }

    /**
     * 播放完成回调，jni层调用
     */
    private void onCompletion() {
        mPrepared = false;
        if (mCompletionListener != null) {
            mCompletionListener.onCompleted();
        }
    }

    /**
     * 定位完成回调，jni层调用
     */
    private void onSeekComplete() {
        mSeeking = false;
        if (mOnSeekCompleteListener != null) {
            mOnSeekCompleteListener.onSeekComplete();
        }
        Log.d("AVMediaPlayer", "onSeekComplete: finish!");
    }

    /**
     * 视频宽高发生变化，jni层调用
     * @param width
     * @param height
     */
    private void onVideoSizeChanged(int width, int height) {
        if (mOnVideoSizeChangedListener != null) {
            mOnVideoSizeChangedListener.onVideoSizeChanged(width, height);
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
     * Interface definition for a callback to be invoked when the media
     * source is ready for playback.
     */
    public interface OnPreparedListener {
        void onPrepared();
    }

    /**
     * Register a callback to be invoked when the media source is ready
     * for playback.
     *
     * @param listener the callback that will be run
     */
    public void setOnPreparedListener(OnPreparedListener listener) {
        mPreparedListener = listener;
    }

    private OnPreparedListener mPreparedListener;

    /**
     * Interface definition for a callback to be invoked when playback of
     * a media source has completed.
     */
    public interface OnCompletionListener {
        void onCompleted();
    }

    /**
     * Register a callback to be invoked when the end of a media source
     * has been reached during playback.
     *
     * @param listener the callback that will be run
     */
    public void setOnCompletionListener(OnCompletionListener listener) {
        mCompletionListener = listener;
    }

    private OnCompletionListener mCompletionListener;

    /**
     * Interface definition of a callback to be invoked indicating
     * the completion of a seek operation.
     */
    public interface OnSeekCompleteListener {
        void onSeekComplete();
    }

    /**
     * Register a callback to be invoked when a seek operation has been
     * completed.
     *
     * @param listener the callback that will be run
     */
    public void setOnSeekCompleteListener(OnSeekCompleteListener listener) {
        mOnSeekCompleteListener = listener;
    }

    private OnSeekCompleteListener mOnSeekCompleteListener;

    /**
     * Interface definition of a callback to be invoked when the
     * video size is first known or updated
     */
    public interface OnVideoSizeChangedListener {
        /**
         * Called to indicate the video size
         *
         * The video size (width and height) could be 0 if there was no video,
         * no display surface was set, or the value was not determined yet.
         *
         * @param width     the width of the video
         * @param height    the height of the video
         */
        void onVideoSizeChanged(int width, int height);
    }

    /**
     * Register a callback to be invoked when the video size is
     * known or updated.
     *
     * @param listener the callback that will be run
     */
    public void setOnVideoSizeChangedListener(OnVideoSizeChangedListener listener) {
        mOnVideoSizeChangedListener = listener;
    }

    private OnVideoSizeChangedListener mOnVideoSizeChangedListener;


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

    private OnErrorListener mErrorListener;

}
