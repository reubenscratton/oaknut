//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

WorkerWeb::WorkerWeb(const string& workerName) {
    string jsfile = workerName + ".js";
    _worker = emscripten_create_worker(jsfile.data());
}
    
WorkerWeb::~WorkerWeb() {
    emscripten_destroy_worker(_worker);
}

void WorkerWeb::start(const variant& config) {
    ByteBufferStream bb;
    bb.writeVariant(config);
    auto configData = bb._data;
    emscripten_call_worker(_worker, "ww_start", (char*)configData.data, configData.cb, NULL, 0);
}

void WorkerWeb::process(const variant& data_in, std::function<void(const variant&)> callback) {
    ByteBufferStream bb;
    bb.writeVariant(data_in);
    _pending.push_back(callback);
    emscripten_call_worker(_worker, "ww_process", (char*)bb._data.data, bb._data.cb, cback_encode, this);
}

void WorkerWeb::handleCallback(const variant& data_out) {
    auto callback = _pending.front();
    callback(data_out);
    _pending.pop_front();
}
    
void WorkerWeb::stop(std::function<void()> onStop) {
    _onStop = onStop;
    emscripten_call_worker(_worker, "ww_stop", 0, 0, cback_stop, this);
}

void WorkerWeb::cback_encode(char* data, int size, void* arg) {
    ByteBufferStream bb;
    bb._data.attach((uint8_t*)data, size);
    variant v;
    bb.readVariant(&v);
    bb._data.detach();
    ((WorkerWeb*)arg)->handleCallback(v);
}
void WorkerWeb::cback_stop(char* data, int size, void* arg) {
    ((WorkerWeb*)arg)->_onStop();
}


#endif

