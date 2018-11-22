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

static void callback_process(char* data, int size, void* arg) {
    ByteBufferStream bb;
    bb._data.attach((uint8_t*)data, size);
    variant data_out;
    bb.readVariant(&data_out);
    bb._data.detach();
    ((Worker*)arg)->dispatchProcessResult(data_out);
}

void Worker::process(const variant& data_in, std::function<void(const variant&)> callback) {
    ByteBufferStream bb;
    bb.writeVariant(data_in);
    _pending.push_back(callback);
    emscripten_call_worker(_worker, "ww_process", (char*)bb._data.data, bb._data.cb, callback_process, this);
}

void Worker::dispatchProcessResult(const variant& data_out) {
    auto callback = _pending.front();
    callback(data_out);
    _pending.pop_front();
}

void Worker::dispatchStopped() {
    _onStop();
}

static void callback_stop(char* data, int size, void* arg) {
    ((Worker*)arg)->dispatchStopped();
}

void Worker::stop(std::function<void()> onStop) {
    _onStop = onStop;
    emscripten_call_worker(_worker, "ww_stop", 0, 0, callback_stop, this);
}


#endif



#endif

