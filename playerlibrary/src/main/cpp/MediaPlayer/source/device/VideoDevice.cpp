//
// Created by cain on 2018/12/28.
//

#include "VideoDevice.h"

VideoDevice::VideoDevice() {

}

VideoDevice::~VideoDevice() {

}

void VideoDevice::start() {

}

void VideoDevice::stop() {

}

int VideoDevice::onRenderYUV(uint8_t *yData, int yPitch, uint8_t *uData, int uPitch, uint8_t *vData,
                             int vPitch) {
    return 0;
}

int VideoDevice::onRenderRGBA(uint8_t *rgba, int pitch) {
    return 0;
}

void VideoDevice::run() {
    // do nothing
}