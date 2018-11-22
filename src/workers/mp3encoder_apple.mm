//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>


class Mp3Encoder : public WorkerImpl {
public:
    int _sampleRate;
    AudioConverterRef _converter;
    AudioStreamBasicDescription _inFormat;
    AudioStreamBasicDescription _outFormat;
    bytearray _inbuf;
    
    
    void start_(const variant& config) override {
        _sampleRate = config.intVal("sampleRate");
        FillOutASBDForLPCM(_inFormat, _sampleRate, 1, 16, 16, false, false);
        memset(&_outFormat, 0, sizeof(_outFormat));
        _outFormat.mSampleRate = _sampleRate;
        _outFormat.mFormatID = kAudioFormatMPEG4AAC;
        _outFormat.mFormatFlags        = kMPEG4Object_AAC_LC; //kMPEG4Object_AAC_Main;
        _outFormat.mChannelsPerFrame =  1;
        _outFormat.mBytesPerPacket     = 0;
        _outFormat.mBytesPerFrame      = 0;
        _outFormat.mFramesPerPacket    = 1024;
        _outFormat.mBitsPerChannel     = 0;
        _outFormat.mReserved           = 0;

        OSStatus status = AudioConverterNew(&_inFormat, &_outFormat, &_converter);
        assert(status == 0);
    }
    variant process_(const variant& data_in) override {
        const bytearray& bytes = data_in.bytearrayVal();
        _inbuf.append(bytes);
        bytearray output;
        
        while(_inbuf.size() > 0) {
            uint8_t tmp[1024*6];
            AudioBufferList fillBufList;
            fillBufList.mNumberBuffers = 1;
            fillBufList.mBuffers[0].mNumberChannels = _outFormat.mChannelsPerFrame;
            fillBufList.mBuffers[0].mDataByteSize = sizeof(tmp);
            fillBufList.mBuffers[0].mData = tmp;
            UInt32 ioOutputDataPacketSize = 1;// ??? wtf is this parameter 
            _inConsumed = 0;
            OSStatus error = AudioConverterFillComplexBuffer(_converter, s_inputDataProc, (void*)this, &ioOutputDataPacketSize, &fillBufList, NULL);
            if (_inConsumed > 0) {
                _inbuf.erase(0, _inConsumed);
                _inConsumed = 0;
            }
            if (error == -123) { // not an error, just ran out of data
                break;
            }
            assert(error == 0);
            if (error) {
                app.log("Error: %d", error);
                break;
            }
            if (ioOutputDataPacketSize > 0) {
                output.append(tmp, fillBufList.mBuffers[0].mDataByteSize);
            } else {
                break;
            }
        }
        
        // Prefix an ADTS header
        if (output.size() > 0) {
            int profile = 2; // AAC_LC
            int channel_config_ = 1; //
            int freqIdx = 7;  //7=22050 6=24000 5=32000, 4=44.1KHz, 2=64000 0=96000
            uint32_t size = output.size() + 7;
            uint8_t adts[7];
            adts[0] = 0xff;
            adts[1] = 0xf1;
            adts[2] = ((profile - 1) << 6) + (freqIdx << 2) +
            (channel_config_ >> 2);
            adts[3] = ((channel_config_ & 0x3) << 6) + (size >> 11);
            adts[4] = (size & 0x7ff) >> 3;
            adts[5] = ((size & 7) << 5) + 0x1f;
            adts[6] = 0xfc;
            output.insert(0, adts, 7);
        }
        return output;
    }
    
    int _inConsumed;
    
    OSStatus inputDataProc(UInt32 *ioNumberDataPackets, AudioBufferList *ioData) {
        if (_inConsumed > 0) {
            _inbuf.erase(0, _inConsumed);
            _inConsumed = 0;
        }
        int cbReq = *ioNumberDataPackets * _inFormat.mBytesPerPacket;
        int cbGot = _inbuf.size();
        int cbSend = MIN(cbReq, cbGot);
        if (cbSend <= 0) { // out of data
            ioData->mBuffers[0].mData = NULL;
            ioData->mBuffers[0].mDataByteSize = 0;
            return -123;
        }
        _inConsumed = cbSend;
        *ioNumberDataPackets = cbSend / _inFormat.mBytesPerPacket;
        ioData->mBuffers[0].mData = _inbuf.data();
        ioData->mBuffers[0].mDataByteSize = cbSend;
        return 0;

    }
                           
    static OSStatus s_inputDataProc(AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription * _Nullable *outDataPacketDescription, void *inUserData) {
        Mp3Encoder* worker = (Mp3Encoder*)inUserData;
        return worker->inputDataProc(ioNumberDataPackets, ioData);
    }
    void stop_() override {
        AudioConverterDispose(_converter);
    }

};

DECLARE_DYNCREATE(Mp3Encoder);

#endif
