//
// Created by cain on 2018/12/21.
//

#include "PacketQueue.h"

PacketQueue::PacketQueue() {
    abort_request = 0;
}

PacketQueue::~PacketQueue() {
    abort();
    flush();
}

/**
 * 入队数据包
 * @param pkt
 * @return
 */
int PacketQueue::put(AVPacket *pkt) {
    if (abort_request) {
        return -1;
    }
    AVPacket *packet = av_packet_alloc();
    av_packet_ref(packet, pkt);
    queue.push(packet);
    return 0;
}

/**
 * 入队数据包
 * @param pkt
 * @return
 */
int PacketQueue::pushPacket(AVPacket *pkt) {
    int ret;
    mMutex.lock();
    ret = put(pkt);
    mCondition.signal();
    mMutex.unlock();

    if (ret < 0) {
        av_packet_unref(pkt);
    }

    return ret;
}

int PacketQueue::pushNullPacket(int stream_index) {
    AVPacket pkt1, *pkt = &pkt1;
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;
    pkt->stream_index = stream_index;
    return pushPacket(pkt);
}

/**
 * 刷新数据包
 */
void PacketQueue::flush() {

    mMutex.lock();
    while (!queue.empty()) {
        AVPacket *pkt = queue.front();
        queue.pop();
        av_packet_free(&pkt);
        av_freep(&pkt);
        pkt = NULL;
    }
    mCondition.signal();
    mMutex.unlock();
}

void PacketQueue::flushToKeyPacket() {

    mMutex.lock();

    while (!queue.empty()) {
        AVPacket *pkt = queue.front();
        if (pkt->flags != AV_PKT_FLAG_KEY) {
            queue.pop();
            av_packet_free(&pkt);
            av_freep(&pkt);
            pkt = NULL;
        } else {
            break;
        }
    }
    mCondition.signal();
    mMutex.unlock();
}

/**
 * 队列终止
 */
void PacketQueue::abort() {
    mMutex.lock();
    abort_request = 1;
    mCondition.signal();
    mMutex.unlock();
}

/**
 * 队列开始
 */
void PacketQueue::start() {
    mMutex.lock();
    abort_request = 0;
    mCondition.signal();
    mMutex.unlock();
}

/**
 * 取出数据包
 * @param pkt
 * @return
 */
int PacketQueue::getPacket(AVPacket *pkt) {
    return getPacket(pkt, 1);
}

/**
 * 取出数据包
 * @param pkt
 * @param block
 * @return
 */
int PacketQueue::getPacket(AVPacket *pkt, int block) {
    int ret;

    mMutex.lock();
    for (;;) {
        if (abort_request) {
            ret = -1;
            break;
        }

        if (queue.size() > 0) {
            AVPacket *packet = queue.front();
            if (av_packet_ref(pkt, packet) == 0) {
                queue.pop();
            }
            av_packet_free(&packet);
            av_freep(&packet);
            packet = NULL;
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            mCondition.wait(mMutex);
        }
    }
    mMutex.unlock();
    return ret;
}

int PacketQueue::getPacketSize() {
    Mutex::Autolock lock(mMutex);
    return queue.size();
}
