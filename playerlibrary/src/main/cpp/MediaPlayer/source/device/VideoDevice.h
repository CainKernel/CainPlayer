//
// Created by cain on 2018/12/28.
//

#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <PlayerOption.h>

class VideoDevice : public Runnable {
public:
    VideoDevice();

    virtual ~VideoDevice();

    virtual void start();

    virtual void stop();

    virtual int onRenderYUV(uint8_t *yData, int yPitch,
                            uint8_t *uData, int uPitch,
                            uint8_t *vData, int vPitch);

    virtual int onRenderRGBA(uint8_t *rgba, int pitch);

    virtual void run();
};

#endif //VIDEODEVICE_H
