//
// Created by cain on 2018/12/26.
//

#ifndef AUDIODECODER_H
#define AUDIODECODER_H


#include <decoder/MediaDecoder.h>
#include <PlayerOption.h>

class AudioDecoder : public MediaDecoder {
public:
    AudioDecoder(AVCodecContext *avctx, AVStream *stream, int streamIndex, PlayerState *playerState);

    virtual ~AudioDecoder();

    void initTimeBase();

    void flush() override;

    int getAudioFrame(AVFrame *frame);

private:
    AVPacket *packet;
};


#endif //AUDIODECODER_H
