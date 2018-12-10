//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

Worker::Worker(const string& name) {
    app.log("Worker::Worker(%s)", name.data());
#if PLATFORM_WEB
    string jsfile = name + ".js";
    _worker = emscripten_create_worker(jsfile.data());
#else
    _impl = (WorkerImpl*)Object::createByName(name);
#endif
}
Worker::Worker() {
}
Worker::~Worker() {
    assert(!_started); // should we stop() implicitly on destruction...?
#if PLATFORM_WEB
    emscripten_destroy_worker(_worker);
#endif
}

void WorkerImpl::start_(const variant& config) {
}
void WorkerImpl::stop_() {
}

#if PLATFORM_WEB


#if BUILD_AS_WORKER
extern WorkerImpl* g_worker;

extern "C" void ww_start(char* bytes, int cb) {
    ByteBufferStream bb(new ByteBuffer((uint8_t*)bytes, cb));
    variant config;
    bb.readVariant(&config);
    g_worker->start_(config);
    emscripten_worker_respond(0, 0);
}
extern "C" void ww_process(char* bytes, int cb) {
    ByteBufferStream bb_in(new ByteBuffer((uint8_t*)bytes, cb));
    variant data_in;
    bb_in.readVariant(&data_in);
    variant data_out = g_worker->process_(data_in);
    ByteBufferStream bb_out;
    bb_out.writeVariant(data_out);
    emscripten_worker_respond((char*)bb_out._data.data, bb_out._data.cb);
}

extern "C" void ww_stop(char* data, int size) {
    g_worker->stop_();
    emscripten_worker_respond(0, 0);
}

#endif

#endif
