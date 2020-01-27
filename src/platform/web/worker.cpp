//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

#if BUILD_AS_WORKER


emscripten_worker_respond(0, 0);
#else

void Worker::start(const variant& config) {
    ByteBufferStream bb;
    bb.writeVariant(config);
    auto configData = bb._data;
    emscripten_call_worker(_worker, "ww_start", (char*)configData.data, configData.cb, NULL, 0);
}




#endif



#endif

