//
// Created by cain on 2018/12/27.
//

#ifndef MEDIADECODER_H
#define MEDIADECODER_H

#include <AndroidLog.h>
#include <PlayerOption.h>
#include <queue/PacketQueue.h>
#include <queue/FrameQueue.h>

class MediaDecoder : public Runnable {
public:
    MediaDecoder(AVCodecContext *avctx, AVStream *stream, int streamIndex, PlayerState *playerState);

    virtual ~MediaDecoder();

    virtual void start();

    virtual void stop();

    virtual void flush();

    int pushPacket(AVPacket *pkt);

    int pushNullPacket();

    int getPacketSize();

    int getStreamIndex();

    AVStream *getStream();

    AVCodecContext *getCodecContext();

    virtual void run();

protected:
    PacketQueue *packetQueue;       // 数据包队列
    AVCodecContext *pCodecCtx;
    AVStream *pStream;
    int streamIndex;

    AVPacket pkt;
    AVPacket pkt_temp;

    PlayerState *playerState;
    int reorder_pts;
    int packet_pending;

    int64_t start_pts;
    AVRational start_pts_tb;
    int64_t next_pts;
    AVRational next_pts_tb;
};


#endif //MEDIADECODER_H
