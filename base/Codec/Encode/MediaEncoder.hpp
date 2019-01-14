//
// Created by 刘轩 on 2019/1/11.
//

#ifndef GPUCAMERAVIDEO_MEDIAENCODER_H
#define GPUCAMERAVIDEO_MEDIAENCODER_H

#include <media/NdkMediaCodec.h>
#include <media/NdkMediaMuxer.h>
#include "libyuv.h"
#include "Encoder.hpp"
#include "Looper.h"
#include "EncoderConfig.hpp"

namespace GCVBase {

    class MediaEncoder : Encoder {

    private:
        bool mStop = true;
        bool mIsYUV420SP = false;

        Looper * encoderLooper = NULL;
        EncoderConfig mEncoderConfig;

        AMediaCodec *mVideoMediaCodec = NULL;
        AMediaCodec *mAudioMediaCodec = NULL;
        AMediaMuxer *mMuxer = NULL;
        ssize_t mVideoTrackIndex = -1;
        ssize_t mAudioTrackIndex = -1;
        FILE * saveFile = NULL;

        bool mVideoIsWritten = false; //必须先让视频帧先写入，不然开始会黑屏的
        bool mIsRunning = false;
        int mStartCount = 0;

        void initVideoCodec();
        void initAudioCodec();

        void startMediaMuxer();
        void stopMediaMuxer(AMediaCodec *codec);

    public:
        MediaEncoder(const GCVBase::EncoderConfig &config);

        void startEncoder(std::function<void(void)> startHandler = NULL);
        void pauseEncoder(std::function<void(void)> pauseHandler = NULL);
        void cancelEncoder(std::function<void(void)> cancelHandler = NULL);
        void finishEncoder(std::function<void(void)> finishHandler = NULL);

        template <class T_> void newFrameReadyAtTime(MediaBuffer<T_> * mediaBuffer){
            if(mStop){
                return;
            }

            if (mediaBuffer->mediaType == MediaType::Audio) {
                newFrameEncodeAudio(mediaBuffer);
            }

            if (mediaBuffer->mediaType == MediaType::Video) {
                newFrameEncodeVideo(mediaBuffer);
            }
        }

        void newFrameEncodeAudio(MediaBuffer<uint8_t *> *audioBuffer); //音频数据
        void newFrameEncodeVideo(MediaBuffer<uint8_t *> *videoBuffer); //视频数据

        void recordCodecBuffer(AMediaCodecBufferInfo *bufferInfo, AMediaCodec *codec, int trackIndex);
    };
}


#endif //GPUCAMERAVIDEO_MEDIAENCODER_H