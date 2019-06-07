//
// Created by cain on 2018/12/30.
//

#ifndef GLESDEVICE_H
#define GLESDEVICE_H

#include <device/VideoDevice.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <renderer/EglHelper.h>
#include <renderer/InputRenderNode.h>

class GLESDevice : public VideoDevice {
public:
    GLESDevice();

    virtual ~GLESDevice();

    void surfaceCreated(ANativeWindow *window);

    void terminate() override;

    void terminate(bool releaseContext);

    void onInitTexture(int width, int height, TextureFormat format, BlendMode blendMode,
                       int rotate) override;

    int onUpdateYUV(uint8_t *yData, int yPitch, uint8_t *uData, int uPitch,
                    uint8_t *vData, int vPitch) override;

    int onUpdateARGB(uint8_t *rgba, int pitch) override;

    int onRequestRender(bool flip) override;

private:
    void resetVertices();

    void resetTexVertices();
private:
    Mutex mMutex;
    Condition mCondition;

    ANativeWindow *mWindow;             // Surface窗口
    int mSurfaceWidth;                  // 窗口宽度
    int mSurfaceHeight;                 // 窗口高度
    EGLSurface eglSurface;              // eglSurface
    EglHelper *eglHelper;               // EGL帮助器
    bool mSurfaceReset;                 // 重新设置Surface
    bool mHasSurface;                   // 是否存在Surface
    bool mHaveEGLSurface;               // EGLSurface
    bool mHaveEGlContext;               // 释放资源

    Texture *mVideoTexture;             // 视频纹理
    InputRenderNode *mRenderNode;       // 输入渲染结点
    float vertices[8];                  // 顶点坐标
    float textureVertices[8];           // 纹理坐标
};

#endif //GLESDEVICE_H
