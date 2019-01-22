//
// Created by cain on 2018/12/27.
//

#include "MediaDecoder.h"

MediaDecoder::MediaDecoder(AVCodecContext *avctx, AVStream *stream, int streamIndex, PlayerState *playerState) {
    packetQueue = new PacketQueue();
    this->pCodecCtx = avctx;
    this->pStream = stream;
    this->streamIndex = streamIndex;
    this->playerState = playerState;
}

MediaDecoder::~MediaDecoder() {
    ALOGI("MediaDecoder destructor");
    stop();
    if (packetQueue) {
        packetQueue->flush();
        delete packetQueue;
        packetQueue = NULL;
    }
    if (pCodecCtx) {
        avcodec_close(pCodecCtx);
        avcodec_free_context(&pCodecCtx);
        pCodecCtx = NULL;
    }
    playerState = NULL;
}

void MediaDecoder::start() {
    if (packetQueue) {
        packetQueue->start();
    }
}

void MediaDecoder::stop() {
    if (packetQueue) {
        packetQueue->abort();
    }
}

void MediaDecoder::flush() {
    if (packetQueue) {
        packetQueue->flush();
    }
}

int MediaDecoder::pushPacket(AVPacket *pkt) {
    if (packetQueue) {
        return packetQueue->pushPacket(pkt);
    }
}

int MediaDecoder::pushNullPacket() {
    if (packetQueue != NULL) {
        return packetQueue->pushNullPacket(streamIndex);
    }
    return -1;
}

int MediaDecoder::getPacketSize() {
    return packetQueue ? packetQueue->getPacketSize() : 0;
}

int MediaDecoder::getStreamIndex() {
    return streamIndex;
}

AVStream *MediaDecoder::getStream() {
    return pStream;
}

AVCodecContext *MediaDecoder::getCodecContext() {
    return pCodecCtx;
}

int MediaDecoder::getMemorySize() {
    return packetQueue ? packetQueue->getSize() : 0;
}

int MediaDecoder::hasEnoughPackets() {
    Mutex::Autolock lock(mMutex);
    return (packetQueue == NULL) || (packetQueue->isAbort())
           || (pStream->disposition & AV_DISPOSITION_ATTACHED_PIC)
           || (packetQueue->getPacketSize() > MIN_FRAMES)
              && (!packetQueue->getDuration()
                  || av_q2d(pStream->time_base) * packetQueue->getDuration() > 1.0);
}

void MediaDecoder::run() {
    // do nothing
}

