//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

class WorkerWeb : public Worker {
public:
    worker_handle _worker;
    std::function<void()> _onStop;
    
    WorkerWeb(const string& workerName) {
        string jsfile = workerName + ".js";
        _worker = emscripten_create_worker(jsfile.data());
    }
    
    ~WorkerWeb() {
        emscripten_destroy_worker(_worker);
    }
    void start(const variant& config) override {
        ByteBufferStream bb;
        bb.writeVariant(config);
        auto configData = bb._data;
        emscripten_call_worker(_worker, "ww_start", (char*)configData.data, configData.cb, NULL, 0);
    }
    list<std::function<void(const bytearray&)>> _pending;
    
    void process(const bytearray& data_in, std::function<void(const bytearray&)> callback) override {
        _pending.push_back(callback);
        emscripten_call_worker(_worker, "ww_process", (char*)data_in.data(), data_in.size(), cback_encode, this);
    }
    virtual void handleCallback(const bytearray& data_out) {
        auto callback = _pending.front();
        callback(data_out);
        _pending.pop_front();
    }
    
    void stop(std::function<void()> onStop) override {
        _onStop = onStop;
        emscripten_call_worker(_worker, "ww_stop", 0, 0, cback_stop, this);
    }
    static void cback_encode(char* data, int size, void* arg) {
        ((Worker*)arg)->handleCallback(bytearray((uint8_t*)data, size));
    }
    static void cback_stop(char* data, int size, void* arg) {
        ((Worker*)arg)->_onStop();
    }
};


Worker* Worker::create(const string& name) {
    if (name == "mp3encoder") {
        return new WorkerWeb("lame");
    }
    assert(0);
    return NULL;
};



#endif

