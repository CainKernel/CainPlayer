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
    frame = av_frame_alloc();
}

VideoDecoder::~VideoDecoder() {
    ALOGI("VideoDecoder destructor");
    stop();
    pFormatCtx = NULL;
    if (frameQueue) {
        frameQueue->flush();
        delete frameQueue;
        frameQueue = NULL;
    }
    if (frame) {
        av_frame_free(&frame);
        av_freep(&frame);
        frame = NULL;
    }
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
    MediaDecoder::flush();
    if (frameQueue) {
        frameQueue->flush();
    }
}

int VideoDecoder::getFrameSize() {
    return frameQueue ? frameQueue->getFrameSize() : 0;
}

FrameQueue *VideoDecoder::getFrameQueue() {
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
