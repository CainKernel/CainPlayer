//
// Created by cain on 2018/12/26.
//

#include "AudioDecoder.h"

AudioDecoder::AudioDecoder(AVCodecContext *avctx, AVStream *stream, int streamIndex, PlayerState *playerState)
        : MediaDecoder(avctx, stream, streamIndex, playerState) {
    packet = av_packet_alloc();
}

AudioDecoder::~AudioDecoder() {
    stop();
    if (packet) {
        av_packet_free(&packet);
        av_freep(&packet);
        packet = NULL;
    }
    ALOGI("AudioDecoder destructor");
}

int AudioDecoder::getAudioFrame(AVFrame *frame) {
    int got_frame = 0;
    int ret = 0;

    if (!frame) {
        return AVERROR(ENOMEM);
    }
    av_frame_unref(frame);

    do {

        if (packetQueue->getPacket(packet) < 0) {
            return -1;
        }

        ret = avcodec_send_packet(pCodecCtx, packet);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
            av_packet_unref(packet);
            continue;
        }
        ret = avcodec_receive_frame(pCodecCtx, frame);
        if (ret < 0) {
            av_frame_unref(frame);
            got_frame = 0;
            continue;
        } else {
            got_frame = 1;
        }
    } while (!got_frame);

    return got_frame;
}







