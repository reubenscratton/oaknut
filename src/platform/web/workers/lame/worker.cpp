//
//
#if PLATFORM_WEB

#include <oaknut.h>


#if !BUILD_AS_WORKER
class Mp3Encoder : public Worker {
public:
    Mp3Encoder() : Worker("Mp3Encoder") {
    }
};


#else

#include "../../../../../thirdparty/libmp3lame/lame.h" // <thirdparty/libmp3lame/lame.h> should work but didn't, hey ho


extern "C" {
    
static lame_global_flags* _lame;
static unsigned char outbuf[256*1024];

    void  ww_start(char* data, int size) {
        _lame = lame_init();
        lame_set_num_channels(_lame, 1);
        lame_set_in_samplerate(_lame, 44100);
        lame_set_out_samplerate(_lame, 22050);
        lame_set_quality(_lame, 5); // allegedly 'good' quality and fast running
        lame_init_params(_lame);
        emscripten_worker_respond(0, 0);
    }
    void ww_process(char* data, int size) {
        //int cbEncoded = lame_encode_buffer(_lame, (short*)data, NULL, size/2, outbuf, sizeof(outbuf));
        int cbEncoded = lame_encode_buffer_ieee_float(_lame,
                                                (float*)data, NULL, size/4,
                                                outbuf, sizeof(outbuf));
        
        if (cbEncoded>0) {
            variant data_out(bytearray(outbuf, cbEncoded));
            ByteBufferStream bbOut;
            bbOut.writeVariant(data_out);
            emscripten_worker_respond((char*)bbOut._data.data, bbOut._data.cb);
        } else {
            emscripten_worker_respond(NULL, 0);
        }

    }
    void ww_stop(char* data, int size) {
        emscripten_worker_respond(0, 0);
    }

}

DECLARE_DYNCREATE(Mp3Encoder);

#endif

#endif


