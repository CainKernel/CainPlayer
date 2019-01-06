package com.cgfay.media.Medadata;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import java.util.HashMap;

/**
 * 基于FFmpeg实现的MediaMetadataRetriever
 */
public class AVMediaMetadataRetriever {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("metadata_retriever");
    }

    public AVMediaMetadataRetriever() {
        nativeSetup();
    }

    @Override
    protected void finalize() throws Throwable {
        nativeRelease();
        super.finalize();
    }

    /**
     * 销毁
     */
    public void release() {
        nativeRelease();
    }

    /**
     * 设置数据源
     * @param path
     */
    public void setDataSource(String path) {
        nativeSetDataSource(path);
    }

    /**
     * 获取metedata数据
     * @param key
     * @return
     */
    public String getMetadata(String key) {
        return nativeGetMetadata(key);
    }

    /**
     * 获取metadata数据
     * @param key
     * @param chapter
     * @return
     */
    public String getMetadata(String key, int chapter) {
        return nativeGetMetadata(key, chapter);
    }

    /**
     * 获取媒体数据
     * @return
     */
    public AVMediaMetadata getMetadata() {
        HashMap<String, String> hashMap = nativeGetMetadata();
        if (hashMap != null) {
            AVMediaMetadata mediaMetadata = new AVMediaMetadata(hashMap);
            return mediaMetadata;
        }
        return null;
    }

    /**
     * 提取一帧数据
     * @param timeUs
     * @return
     */
    public Bitmap getFrame(long timeUs) {
        byte[] data = nativeGetFrame(timeUs);
        Bitmap bitmap = null;
        if (data != null) {
            bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
        }
        return bitmap;
    }

    /**
     * 按照特定宽高提取一帧数据
     * @param timeUs
     * @param width
     * @param height
     * @return
     */
    public Bitmap getFrame(long timeUs, int width, int height) {
        byte[] data = nativeGetFrame(timeUs, width, height);
        Bitmap bitmap = null;
        if (data != null) {
            bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
        }
        return bitmap;
    }

    /**
     * 提取专辑/封面图片
     * @return
     */
    public Bitmap getCoverPicture() {
        byte[] data = nativeGetCoverPicture();
        Bitmap bitmap = null;
        if (data != null) {
            bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
        }
        return bitmap;
    }

    // native 方法
    private native void nativeSetup();
    private native void nativeRelease();
    private native void nativeSetDataSource(String path);
    private native String nativeGetMetadata(String key);
    private native String nativeGetMetadata(String key, int chapter);
    private native HashMap<String, String> nativeGetMetadata();
    private native byte[] nativeGetFrame(long timeUs);
    private native byte[] nativeGetFrame(long timeUs, int width, int height);
    private native byte[] nativeGetCoverPicture();
}
