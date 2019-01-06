//
// Created by cain on 2018/12/27.
//

#include "MediaDecoder.h"

MediaDecoder::MediaDecoder(AVCodecContext *avctx, AVStream *stream, int streamIndex, PlayerState *playerState) {
    memset(&pkt, 0, sizeof(AVPacket));
    pkt_temp = pkt;
    packetQueue = new PacketQueue();
    this->pCodecCtx = avctx;
    this->pStream = stream;
    this->streamIndex = streamIndex;
    this->playerState = playerState;
    reorder_pts = -1;
    packet_pending = 0;
    start_pts = AV_NOPTS_VALUE;
    memset(&start_pts_tb, 0, sizeof(AVRational));
    next_pts = AV_NOPTS_VALUE;
    memset(&next_pts_tb, 0, sizeof(next_pts_tb));
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
    packetQueue->flush();
}

int MediaDecoder::pushPacket(AVPacket *pkt) {
    return packetQueue->pushPacket(pkt);
}

int MediaDecoder::pushNullPacket() {
    return packetQueue->pushNullPacket(streamIndex);
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

void MediaDecoder::run() {
    // do nothing
}

