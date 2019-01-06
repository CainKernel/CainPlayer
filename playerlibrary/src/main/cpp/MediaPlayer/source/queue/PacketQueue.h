//
// Created by cain on 2018/12/21.
//

#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H

#include <queue>
#include <Mutex.h>
#include <Condition.h>

extern "C" {
#include <libavcodec/avcodec.h>
};

class PacketQueue {
public:
    PacketQueue();

    virtual ~PacketQueue();

    // 入队数据包
    int pushPacket(AVPacket *pkt);

    // 入队空数据包
    int pushNullPacket(int stream_index);

    // 刷新
    void flush();

    // 刷新到关键帧
    void flushToKeyPacket();

    // 终止
    void abort();

    // 开始
    void start();

    // 获取数据包
    int getPacket(AVPacket *pkt);

    // 获取数据包
    int getPacket(AVPacket *pkt, int block);

    int getPacketSize();
private:
    int put(AVPacket *pkt);

private:
    Mutex mMutex;
    Condition mCondition;
    std::queue<AVPacket *> queue;
    int abort_request;
};


#endif //PACKETQUEUE_H
