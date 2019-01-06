//
// Created by cain on 2018/12/30.
//

#include "GLESDevice.h"

GLESDevice::GLESDevice() {

}

GLESDevice::~GLESDevice() {

}

void GLESDevice::start() {
    VideoDevice::start();
}

void GLESDevice::stop() {
    VideoDevice::stop();
}

int GLESDevice::onRenderYUV(uint8_t *yData, int yPitch, uint8_t *uData, int uPitch, uint8_t *vData,
                            int vPitch) {
    return VideoDevice::onRenderYUV(yData, yPitch, uData, uPitch, vData, vPitch);
}

int GLESDevice::onRenderRGBA(uint8_t *rgba, int pitch) {
    return VideoDevice::onRenderRGBA(rgba, pitch);
}

void GLESDevice::run() {
    VideoDevice::run();
}