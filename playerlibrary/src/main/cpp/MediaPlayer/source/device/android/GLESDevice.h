//
// Created by cain on 2018/12/30.
//

#ifndef GLESDEVICE_H
#define GLESDEVICE_H

#include <device/VideoDevice.h>


class GLESDevice : public VideoDevice {
public:
    GLESDevice();

    virtual ~GLESDevice();

    void start() override;

    void stop() override;

    int onRenderYUV(uint8_t *yData, int yPitch, uint8_t *uData, int uPitch,
                    uint8_t *vData, int vPitch) override;

    int onRenderRGBA(uint8_t *rgba, int pitch) override;

    void run() override;

};

#endif //GLESDEVICE_H
