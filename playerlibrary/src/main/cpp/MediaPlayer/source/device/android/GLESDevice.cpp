//
// Created by cain on 2018/12/30.
//

#include <renderer/YUV420PRenderer.h>
#include <renderer/BGRARenderer.h>
#include "GLESDevice.h"

GLESDevice::GLESDevice() {
    glThread = NULL;
    mWindow = NULL;
    eglSurface = EGL_NO_SURFACE;
    eglHelper = NULL;

    mShouldExit = false;
    mExited = true;
    mRequestPaused = false;
    mPaused = false;
    mHasSurface = false;
    mSurfaceIsBad = false;
    mWaitingForSurface = false;
    mHaveEglContext = false;
    mHaveEglSurface = false;
    mFinishedCreatingEglSurface = false;
    mShouldReleaseEglContext = false;
    mWidth = 0;
    mHeight = 0;
    mRequestRender = true;
    mRenderComplete = false;
    mSizeChanged = true;
    mPreserveEGLContextOnPause = false;

    mVideoTexture = (Texture *) malloc(sizeof(Texture));
    memset(mVideoTexture, 0, sizeof(Texture));
    mRenderer = NULL;
}

GLESDevice::~GLESDevice() {
    stop();
}

void GLESDevice::surfaceCreated(ANativeWindow *window) {
    Mutex::Autolock lock(mMutex);
    if (mWindow != NULL) {
        ANativeWindow_release(mWindow);
        mWindow = NULL;
    }
    mWindow = window;
    mHasSurface = true;
    mFinishedCreatingEglSurface = false;
    mCondition.broadcast();
    // 等待创建完成
    while (mWaitingForSurface
           && !mFinishedCreatingEglSurface
           && !mExited) {
        mCondition.wait(mMutex);
    }
}

void GLESDevice::surfaceChanged(int width, int height) {
    Mutex::Autolock lock(mMutex);
    mWidth = width;
    mHeight = height;
    mSizeChanged = true;
    mRequestRender = true;
    mRenderComplete = false;
    mCondition.broadcast();
    // 等待窗口改变完成
    while (!mExited && !mPaused && !mRenderComplete
           && ableToDraw()) {
        mCondition.wait(mMutex);
    }
}

void GLESDevice::surfaceDestroyed() {
    Mutex::Autolock lock(mMutex);
    mHasSurface = false;
    mCondition.broadcast();
    // 等待Surface销毁操作处理完成
    while((!mWaitingForSurface) && (!mExited)) {
        mCondition.wait(mMutex);
    }
}

void GLESDevice::start() {
    Mutex::Autolock lock(mMutex);
    if (glThread == NULL) {
        glThread = new Thread(this);
        glThread->start();
    }
    mExited = false;
    mCondition.broadcast();
}

void GLESDevice::pause() {
    Mutex::Autolock lock(mMutex);
    mRequestPaused = true;
    mCondition.broadcast();
    // 等待暂停完成
    while ((!mExited) && (!mPaused)) {
        mCondition.wait(mMutex);
    }
}

void GLESDevice::resume() {
    Mutex::Autolock lock(mMutex);
    mRequestPaused = false;
    mRequestRender = true;
    mRenderComplete = false;
    mCondition.broadcast();
    // 等待启动完成
    while ((!mExited) && mPaused && (!mRenderComplete)) {
        mCondition.wait(mMutex);
    }
}

void GLESDevice::stop() {
    Mutex::Autolock lock(mMutex);
    mShouldExit = true;
    mCondition.broadcast();
    while (!mExited) {
        mCondition.wait(mMutex);
    }
    if (glThread != NULL) {
        glThread->join();
        delete glThread;
        glThread = NULL;
    }
}

void GLESDevice::onInitTexture(int width, int height, TextureFormat format, BlendMode blendMode) {
    Mutex::Autolock lock(mMutex);
    mVideoTexture->width = width;
    mVideoTexture->height = height;
    mVideoTexture->format = format;
    mVideoTexture->blendMode = blendMode;
    mVideoTexture->direction = FLIP_NONE;
    if (format == FMT_YUV420P) {
        mRenderer = new YUV420PRenderer();
    } else if (format == FMT_ARGB) {
        mRenderer = new BGRARenderer();
    } else {
        mRenderer = NULL;
    }
    mCondition.broadcast();
}

int GLESDevice::onUpdateYUV(uint8_t *yData, int yPitch, uint8_t *uData, int uPitch, uint8_t *vData,
                            int vPitch) {
    Mutex::Autolock lock(mMutex);

    mVideoTexture->pitches[0] = yPitch;
    mVideoTexture->pitches[1] = uPitch;
    mVideoTexture->pitches[2] = vPitch;

    mVideoTexture->pixels[0] = yData;
    mVideoTexture->pixels[1] = uData;
    mVideoTexture->pixels[2] = vData;

    mCondition.broadcast();
    return 0;
}

int GLESDevice::onUpdateARGB(uint8_t *rgba, int pitch) {
    Mutex::Autolock lock(mMutex);
    mVideoTexture->pitches[0] = pitch;
    mVideoTexture->pixels[0] = rgba;
    mCondition.broadcast();
    return 0;
}

int GLESDevice::onRequestRender(FlipDirection direction) {
    Mutex::Autolock lock(mMutex);
    mVideoTexture->direction = direction;
    mRequestRender = true;
    mCondition.broadcast();
    return 0;
}

bool GLESDevice::ableToDraw() {
    return mHaveEglContext && mHaveEglSurface && readyToDraw();
}

bool GLESDevice::readyToDraw() {
    return (!mPaused) && mHasSurface && (!mSurfaceIsBad)
           && (mWidth > 0) && (mHeight > 0)
           && mRequestRender;
}

void GLESDevice::stopEglSurface() {
    if (mHaveEglSurface) {
        if (eglHelper != NULL) {
            eglHelper->destroySurface(eglSurface);
        }
        mHaveEglSurface = false;
        eglSurface = EGL_NO_SURFACE;
    }
    if (mWindow != NULL) {
        ANativeWindow_release(mWindow);
        mWindow = NULL;
    }
    mCondition.broadcast();
}

void GLESDevice::stopEglContext() {
    if (mHaveEglContext) {
        if (eglHelper != NULL) {
            eglHelper->release();
        }
        mHaveEglContext = false;
    }
    mCondition.broadcast();
}

void GLESDevice::run() {
    guardedRun();
    mMutex.lock();
    mExited = true;
    mCondition.broadcast();
    mMutex.unlock();
}

void GLESDevice::guardedRun() {
    if (eglHelper == NULL) {
        eglHelper = new EglHelper();
    }
    mHaveEglContext = false;
    mHaveEglSurface = false;

    bool createEglContext = false;
    bool createEglSurface = false;
    bool lostEglContext = false;
    bool sizeChanged = false;
    bool askedToReleaseEglContext = false;
    int w = 0;
    int h = 0;

    bool exitRun = false;

    Mutex renderMutex;
    while (true) {

        renderMutex.lock();
        while (true) {

            // 退出渲染线程
            if (mShouldExit) {
                exitRun = true;
                break;
            }

            // 更新暂停状态
            bool pausing = false;
            if (mPaused != mRequestPaused) {
                pausing = mRequestPaused;
                mPaused = mRequestPaused;
                mCondition.broadcast();
                ALOGD("mPaused != mRequestPaused");
            }

            // 释放EGLContext 上下文
            if (mShouldReleaseEglContext) {
                stopEglSurface();
                stopEglContext();
                mShouldReleaseEglContext = false;
                askedToReleaseEglContext = true;
                ALOGD("mShouldReleaseEglContext");
            }

            // 丢失EGLContext上下文处理
            if (lostEglContext) {
                stopEglSurface();
                stopEglContext();
                lostEglContext = false;
                ALOGD("lostEglContext");
            }

            // 处于暂停状态，则释放EGLSurface
            if (pausing && mHaveEglSurface) {
                ALOGD("pausing && mHaveEglSurface");
                stopEglSurface();
            }

            // 处于暂停状态，是否释放EGLContext上下文
            if (pausing && mHaveEglContext) {
                ALOGD("pausing && mHaveEglContext");
                if (!mPreserveEGLContextOnPause) {
                    stopEglContext();
                }
            }

            // 判断是否丢失SurfaceView 的surface
            if ((!mHasSurface) && (!mWaitingForSurface)) {
                ALOGD("(!mHasSurface) && (!mWaitingForSurface)");
                if (mHaveEglSurface) {
                    stopEglSurface();
                }
                mWaitingForSurface = true;
                mSurfaceIsBad = false;
                mCondition.broadcast();
            }

            // 是否获得了SurfaceView 的surface
            if (mHasSurface && mWaitingForSurface) {
                mWaitingForSurface = false;
                mCondition.broadcast();
            }

            // 如果没有EGLContext，尝试创建一个
            if (!mHaveEglContext) {
                if (askedToReleaseEglContext) {
                    askedToReleaseEglContext = false;
                } else {
                    mHaveEglContext = eglHelper->init(NULL, FLAG_TRY_GLES3);
                    if (mHaveEglContext) {
                        createEglContext = true;
                    }
                    mCondition.broadcast();
                }
            }

            // 判断是否准备好绘制了
            if (readyToDraw()) {
                if (mHaveEglContext && !mHaveEglSurface) {
                    mHaveEglSurface = true;
                    createEglSurface = true;
                    sizeChanged = true;
                }
                if (mHaveEglSurface) {
                    if (mSizeChanged) {
                        sizeChanged = true;
                        w = mWidth;
                        h = mHeight;
                        createEglSurface = true;
                        mSizeChanged = false;
                    }
                    mRequestRender = false;
                    mCondition.broadcast();
                    break;
                }
            }
            // 等待广播
            mCondition.wait(renderMutex);
        }
        renderMutex.unlock();

        // 退出线程
        if (exitRun) {
            break;
        }

        // 创建EGLSurface
        if (createEglSurface) {
            // 如果EGLSurface还没创建，或者是EGLSurface坏掉了，则进入创建EGLSurface阶段
            if (eglSurface == EGL_NO_SURFACE || mSurfaceIsBad) {
                // 如果仅仅是EGLSurface坏掉了，则先停止EGLSurface
                if (mSurfaceIsBad && eglSurface != EGL_NO_SURFACE) {
                    stopEglSurface();
                }

                // 重新创建EGLSureface
                if ((eglSurface == EGL_NO_SURFACE) && (mWindow != NULL)) {
                    eglSurface = eglHelper->createSurface(mWindow);
                    if (eglSurface != NULL && eglSurface != EGL_NO_SURFACE) {
                        mFinishedCreatingEglSurface = true;
                        mSurfaceIsBad = false;
                        mCondition.broadcast();
                    } else {
                        // 如果此时创建不成功，表示EGLSurface坏掉了，继续下一轮循环，直到创建成功为止
                        mSurfaceIsBad = true;
                        mCondition.broadcast();
                        continue;
                    }
                } else {
                    // 如果ANativeWindow不存在或EGLSurface != EGL_NO_SURFACE，则表示Surface坏掉了，
                    // 此时需要继续下一轮循环，直到创建成功为止
                    mSurfaceIsBad = true;
                    mCondition.broadcast();
                    continue;
                }
            }
            createEglSurface = false;
        }

        // 切换到当前状态
        if (eglSurface != EGL_NO_SURFACE) {
            eglHelper->makeCurrent(eglSurface);
        }

        // 创建EGLContext回调处理
        if (createEglContext) {
            onSurfaceCreated();
            createEglContext = false;
        }

        // 处理窗口发生变化操作
        if (sizeChanged) {
            onSurfaceChanged(w, h);
            sizeChanged = false;
        }

        // 绘制操作
        onDrawFrame();

        // 交换显示到前台
        int swapError = eglHelper->swapBuffers(eglSurface);
        // 处理交换结果
        switch (swapError) {
            case EGL_SUCCESS: {
                break;
            }

            case EGL_CONTEXT_LOST: {
                lostEglContext = true;
                break;
            }

            // 其他信息，则表示Surface已经坏掉了
            default: {
                mSurfaceIsBad = true;
                mCondition.broadcast();
                break;
            }
        }
    }

    // 渲染线程结束，需要释放EGL资源
    renderMutex.lock();
    stopEglSurface();
    stopEglContext();
    if (eglHelper != NULL) {
        delete eglHelper;
        eglHelper = NULL;
    }
    renderMutex.unlock();
}

void GLESDevice::onSurfaceCreated() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GLESDevice::onSurfaceChanged(int width, int height) {
    glViewport(0, 0, width, height);
}

void GLESDevice::onDrawFrame() {
    if (mRenderer == NULL) {
        return;
    }
    mRenderer->onInit(mVideoTexture);
    mRenderer->renderTexture(mVideoTexture);
}