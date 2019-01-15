//
// Created by cain on 2018/12/30.
//

#ifndef GLESDEVICE_H
#define GLESDEVICE_H

#include <device/VideoDevice.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <renderer/GLUtils.h>
#include <renderer/EglHelper.h>

class GLESDevice : public VideoDevice {
public:
    GLESDevice();

    virtual ~GLESDevice();

    void surfaceCreated(ANativeWindow *window);

    void surfaceChanged(int width, int height);

    void surfaceDestroyed();

    void start() override;

    void pause() override;

    void resume() override;

    void stop() override;

    void onInitTexture(int width, int height, TextureFormat format, BlendMode blendMode) override;

    int onUpdateYUV(uint8_t *yData, int yPitch, uint8_t *uData, int uPitch,
                    uint8_t *vData, int vPitch) override;

    int onUpdateARGB(uint8_t *rgba, int pitch) override;

    int onRequestRender(FlipDirection direction) override;

    void run() override;

private:
    void guardedRun();

    bool ableToDraw();

    bool readyToDraw();

    void stopEglSurface();

    void stopEglContext();

    void onSurfaceCreated();

    void onSurfaceChanged(int width, int height);

    void onDrawFrame();

private:
    Mutex mMutex;
    Condition mCondition;
    Thread *glThread;                   // gl渲染线程

    ANativeWindow *mWindow;             // Surface窗口
    EGLSurface eglSurface;              // eglSurface
    EglHelper *eglHelper;               // EGL帮助器

    bool mShouldExit;                   // 请求退出
    bool mExited;                       // 退出状态
    bool mRequestPaused;                // 请求暂停
    bool mPaused;                       // 处于暂停状态
    bool mHasSurface;                   // Surface是否存在
    bool mSurfaceIsBad;                 // Surface坏掉了
    bool mWaitingForSurface;            // 等待Surface创建
    bool mHaveEglContext;               // 是否存在EGLContext
    bool mHaveEglSurface;               // 是否有存在EGLSurface
    bool mFinishedCreatingEglSurface;   // EGLSurface创建成功
    bool mShouldReleaseEglContext;      // 是否应该释放EGLContext
    int mWidth;                         // Surface宽度
    int mHeight;                        // Surface高度
    bool mRequestRender;                // 请求渲染标志
    bool mRenderComplete;               // 渲染完成标志
    bool mSizeChanged;                  // Surface大小发生变化标志
    bool mPreserveEGLContextOnPause;    // 处于暂停状态时是否保持EGLContext

    Texture *mVideoTexture;             // 视频纹理
    Renderer *mRenderer;                // 渲染器
};

#endif //GLESDEVICE_H
