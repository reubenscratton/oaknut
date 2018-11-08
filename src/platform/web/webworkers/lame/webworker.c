//
//
#include <emscripten/emscripten.h>
#include "libmp3lame/lame.h"

//extern "C" {
    
    lame_global_flags* _lame;
    unsigned char outbuf[256*1024];

    void ww_start(char* data, int size) {
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
        emscripten_worker_respond((cbEncoded>0)?outbuf:NULL, cbEncoded);
    }
    void ww_stop(char* data, int size) {
        emscripten_worker_respond(0, 0);
    }

//}

