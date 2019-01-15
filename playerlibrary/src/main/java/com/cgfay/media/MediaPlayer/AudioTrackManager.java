package com.cgfay.media.MediaPlayer;

import android.media.AudioFormat;
import android.media.AudioTrack;
import android.util.Log;

/**
 * 音频播放器管理器，Native层调用
 */
public class AudioTrackManager {

    private static final String TAG = "AudioTrackManager";

    // AudioTrack播放器
    private static AudioTrack mAudioTrack;

    /**
     * 初始化
     */
    public static void initialize() {
        if (mAudioTrack != null) {
            closeAudioPlayer();
        }
        mAudioTrack = null;
    }

    /**
     * 打开音频播放器，JNI层调用
     * @param sampleRate
     * @param is16Bit
     * @param isStereo
     * @param desiredFrames
     * @return
     */
    public static int openAudoPlayer(int sampleRate, boolean is16Bit, boolean isStereo,
                                     int desiredFrames) {
        initialize();
        int channelConfig = isStereo ? AudioFormat.CHANNEL_IN_STEREO : AudioFormat.CHANNEL_IN_MONO;
        int audioFormat = is16Bit ? AudioFormat.ENCODING_PCM_16BIT : AudioFormat.ENCODING_PCM_8BIT;
        int frameSize = (isStereo ? 2 : 1) * (is16Bit ? 2 : 1);

        Log.v(TAG, "audio device: wanted "
                + (isStereo ? "stereo" : "mono")
                + " " + (is16Bit ? "16-bit" : "8-bit")
                + " " + (sampleRate / 1000f) + "kHz, "
                + desiredFrames + " frames buffer");

        desiredFrames = Math.max(desiredFrames,
                (AudioTrack.getMinBufferSize(sampleRate, channelConfig, audioFormat)
                        + frameSize - 1) / frameSize);

        if (mAudioTrack == null) {
            mAudioTrack = new AudioTrack(android.media.AudioManager.STREAM_MUSIC, sampleRate,
                    channelConfig, audioFormat, desiredFrames * frameSize, AudioTrack.MODE_STREAM);
            if (mAudioTrack.getState() != AudioTrack.STATE_INITIALIZED) {
                Log.e(TAG, "Failed during initialization of Audio Track");
                mAudioTrack = null;
                return -1;
            }

            mAudioTrack.play();
        }

        Log.v(TAG, "audio device: got "
                + ((mAudioTrack.getChannelCount() >= 2) ? "stereo" : "mono")
                + " " + ((mAudioTrack.getAudioFormat() == AudioFormat.ENCODING_PCM_16BIT) ? "16-bit" : "8-bit")
                + " " + (mAudioTrack.getSampleRate() / 1000f) + "kHz, "
                + desiredFrames + " frames buffer");

        return 0;
    }

    /**
     * 关闭音频播放器，JNI层调用
     */
    public static void closeAudioPlayer() {
        if (mAudioTrack != null) {
            mAudioTrack.stop();
            mAudioTrack.release();
            mAudioTrack = null;
        }
    }

    /**
     * 播放输出short数据，JNI层调用
     * @param buffer
     */
    public static void audioPlayShortBuffer(short[] buffer) {
        if (mAudioTrack == null) {
            Log.e(TAG, "Attempted to make audio call with uninitialized audio!");
            return;
        }

        for (int i = 0; i < buffer.length; ) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // do nothing
                }
            } else {
                Log.w(TAG, "audio device: error return from write(short)");
                return;
            }
        }
    }

    /**
     * 播放输出字节数据，JNI层调用
     * @param buffer
     */
    public static void audioPlayByteBuffer(byte[] buffer) {
        if (mAudioTrack == null) {
            Log.e(TAG, "Attempted to make audio call with uninitialized audio!");
            return;
        }

        for (int i = 0; i < buffer.length; ) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // Nom nom
                }
            } else {
                Log.w(TAG, "audio device: error return from write(byte)");
                return;
            }
        }
    }
}
