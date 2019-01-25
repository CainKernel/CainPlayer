//
// Created by cain on 2018/12/26.
//

#include "VideoDecoder.h"

VideoDecoder::VideoDecoder(AVFormatContext *pFormatCtx, AVCodecContext *avctx,
                           AVStream *stream, int streamIndex, PlayerState *playerState)
        : MediaDecoder(avctx, stream, streamIndex, playerState) {
    this->pFormatCtx = pFormatCtx;
    frameQueue = new FrameQueue(VIDEO_QUEUE_SIZE, 1);
    decodeThread = NULL;
    masterClock = NULL;
}

VideoDecoder::~VideoDecoder() {
    ALOGI("VideoDecoder destructor");
    stop();
    mMutex.lock();
    pFormatCtx = NULL;
    if (frameQueue) {
        frameQueue->flush();
        delete frameQueue;
        frameQueue = NULL;
    }
    masterClock = NULL;
    mMutex.unlock();
}

void VideoDecoder::setMasterClock(MediaClock *masterClock) {
    Mutex::Autolock lock(mMutex);
    this->masterClock = masterClock;
}

void VideoDecoder::start() {
    MediaDecoder::start();
    if (frameQueue) {
        frameQueue->start();
    }
    if (!decodeThread) {
        decodeThread = new Thread(this);
        decodeThread->start();
    }
}

void VideoDecoder::stop() {
    MediaDecoder::stop();
    if (frameQueue) {
        frameQueue->abort();
    }
    if (decodeThread) {
        decodeThread->join();
        delete decodeThread;
        decodeThread = NULL;
    }
}

void VideoDecoder::flush() {
    mMutex.lock();
    MediaDecoder::flush();
    if (frameQueue) {
        frameQueue->flush();
    }
    mCondition.signal();
    mMutex.unlock();
}

int VideoDecoder::getFrameSize() {
    Mutex::Autolock lock(mMutex);
    return frameQueue ? frameQueue->getFrameSize() : 0;
}

FrameQueue *VideoDecoder::getFrameQueue() {
    Mutex::Autolock lock(mMutex);
    return frameQueue;
}

void VideoDecoder::run() {
    decodeVideo();
}

/**
 * 解码视频数据包并放入帧队列
 * @return
 */
int VideoDecoder::decodeVideo() {
    AVFrame *frame = av_frame_alloc();
    Frame *vp;
    int got_picture;
    int ret = 0;

    AVRational tb = pStream->time_base;
    AVRational frame_rate = av_guess_frame_rate(pFormatCtx, pStream, NULL);

    if (!frame) {
        return AVERROR(ENOMEM);
    }

    AVPacket *packet = av_packet_alloc();
    if (!packet) {
        return AVERROR(ENOMEM);
    }

    for (;;) {

        if (playerState->abortRequest) {
            ret = -1;
            break;
        }

        if (packetQueue->getPacket(packet) < 0) {
            return -1;
        }

        // 送去解码
        ret = avcodec_send_packet(pCodecCtx, packet);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
            av_packet_unref(packet);
            continue;
        }

        // 得到解码帧
        ret = avcodec_receive_frame(pCodecCtx, frame);
        if (ret < 0 && ret != AVERROR_EOF) {
            av_frame_unref(frame);
            av_packet_unref(packet);
            continue;
        } else {
            got_picture = 1;
            // 丢帧处理
            if (masterClock != NULL) {
                double dpts = NAN;

                if (frame->pts != AV_NOPTS_VALUE) {
                    dpts = av_q2d(pStream->time_base) * frame->pts;
                }
                // 计算视频帧的长宽比
                frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(pFormatCtx, pStream,
                                                                          frame);
                // 是否需要做舍帧操作
                if (playerState->frameDrop > 0 ||
                    (playerState->frameDrop > 0 && playerState->syncType != AV_SYNC_VIDEO)) {
                    if (frame->pts != AV_NOPTS_VALUE) {
                        double diff = dpts - masterClock->getClock();
                        if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
                            diff < 0 && packetQueue->getPacketSize() > 0) {
                            av_frame_unref(frame);
                            got_picture = 0;
                        }
                    }
                }
            }
        }

        if (got_picture) {

            // 取出帧
            if (!(vp = frameQueue->peekWritable())) {
                ret = -1;
                break;
            }

            // 复制参数
            vp->uploaded = 0;
            vp->width = frame->width;
            vp->height = frame->height;
            vp->format = frame->format;
            vp->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
            vp->duration = frame_rate.num && frame_rate.den
                           ? av_q2d((AVRational){frame_rate.den, frame_rate.num}) : 0;
            av_frame_move_ref(vp->frame, frame);

            // 入队帧
            frameQueue->pushFrame();
        }

        // 释放数据包和缓冲帧的引用，防止内存泄漏
        av_frame_unref(frame);
        av_packet_unref(packet);
    }

    av_frame_free(&frame);
    av_free(frame);
    frame = NULL;

    av_packet_free(&packet);
    av_free(packet);
    packet = NULL;

    ALOGD("video decode thread exit!");
    return ret;
}
