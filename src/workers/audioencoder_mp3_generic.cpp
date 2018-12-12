//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if USE_WORKER_Mp3Encoder

// Workers are built separately for web, they shouldn't be compiled into the main .js
#if !(PLATFORM_WEB && !BUILD_AS_WORKER)

#include <oaknut.h>
#include "../thirdparty/libmp3lame/lame.h" // <thirdparty/libmp3lame/lame.h> should work but didn't, hey ho


#define OUTBUF_SIZE 256*1024

class Mp3Encoder : public WorkerImpl {
public:
    lame_global_flags* _lame;
    uint8_t* _outbuf;

    Mp3Encoder() : WorkerImpl() {
    }

    void start_(const variant& config) override {
        _lame = lame_init();
        lame_set_num_channels(_lame, 1);
        lame_set_in_samplerate(_lame, 44100);
        lame_set_out_samplerate(_lame, 22050);
        lame_set_quality(_lame, 5); // allegedly 'good' quality and fast running
        lame_init_params(_lame);
        _outbuf = (uint8_t*)malloc(OUTBUF_SIZE);
    }
    variant process_(const variant& data_in) override {
        const bytearray& bytes = data_in.bytearrayVal();
        
        //int cbEncoded = lame_encode_buffer(_lame, (short*)data, NULL, size/2, outbuf, sizeof(outbuf));
        int cbEncoded = lame_encode_buffer_ieee_float(_lame,
                                                      (float*)bytes.data(), NULL, bytes.size()/4,
                                                      _outbuf, OUTBUF_SIZE);
        return variant(bytearray(_outbuf, cbEncoded));
    }
    void stop_() override {
        free(_outbuf);
        _outbuf = NULL;
    }

};




DECLARE_WORKER_IMPL(Mp3Encoder, "Mp3Encoder");

#include "../thirdparty/libmp3lame/bitstream.c"
#include "../thirdparty/libmp3lame/encoder.c"
#include "../thirdparty/libmp3lame/fft.c"
#include "../thirdparty/libmp3lame/gain_analysis.c"
#include "../thirdparty/libmp3lame/lame.c"
#include "../thirdparty/libmp3lame/newmdct.c"
#include "../thirdparty/libmp3lame/presets.c"
#include "../thirdparty/libmp3lame/psymodel.c"
#include "../thirdparty/libmp3lame/quantize_pvt.c"
#include "../thirdparty/libmp3lame/quantize.c"
#include "../thirdparty/libmp3lame/reservoir.c"
#include "../thirdparty/libmp3lame/set_get.c"
#include "../thirdparty/libmp3lame/tables.c"
#include "../thirdparty/libmp3lame/takehiro.c"
#include "../thirdparty/libmp3lame/util.c"
#include "../thirdparty/libmp3lame/vbrquantize.c"
#include "../thirdparty/libmp3lame/VbrTag.c"
#include "../thirdparty/libmp3lame/version.c"

#endif
#endif

