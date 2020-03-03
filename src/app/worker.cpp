//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


Worker::Worker(const string& name) {
    app->log("Worker::Worker(%s)", name.c_str());
#if PLATFORM_WEB
    string jsfile = name + ".js";
    _worker = emscripten_create_worker(jsfile.c_str());
#else
    _impl = (WorkerImpl*)Object::createByName(name);
#endif
}
Worker::~Worker() {
#if PLATFORM_WEB
    emscripten_destroy_worker(_worker);
#endif
}


#if PLATFORM_WEB && BUILD_AS_WORKER
extern WorkerImpl* g_worker;

extern "C" void ww_work(char* bytes, int cb) {
    ByteBufferStream bb_in(new ByteBuffer((uint8_t*)bytes, cb));
    variant data_in;
    int32_t msg = 0;
    bb_in.readInt32(&msg);
    bb_in.readVariant(&data_in);
    variant data_out = g_worker->work(msg, data_in);
    if (data_out.isEmpty()) {
        emscripten_worker_respond(0, 0);
    } else {
        ByteBufferStream bb_out;
        bb_out.writeVariant(data_out);
        emscripten_worker_respond((char*)bb_out._data.data, bb_out._data.cb);
    }
}

#endif





Task* Worker::enqueue(int32_t msg, const variant& data_in, std::function<void(const variant&)> callback) {
    
    if (isBusy()) {
        return nullptr;
    }
    _queuedTasks++;
    Task* task = Task::enqueue({
        {Task::Background, [=](variant& data) -> variant {
            return _impl->process_(msg, data_in);
        }},
        {Task::MainThread, [=](variant& result) -> variant {
            if (callback) {
                callback(result);
            }
            _queuedTasks--;
            return variant();
        }},
    });
    return task;
}

bool Worker::isBusy() {
    if (_maxQueueSize > 0) {
        int numQueued = _queuedTasks.fetch_add(0);
        if (numQueued >= _maxQueueSize) {
            return true;
        }
    }
    return false;
}

/*void Worker::drain() {
    if (_current.load()) {
        return;
    }
    _current = _pending.front();
    _pending.pop_front();

#if PLATFORM_WEB
    ByteBufferStream bb;
    bb.writeInt32(msg);
    bb.writeVariant(data_in);
    emscripten_call_worker(_worker, "ww_work", (char*)bb._data.data, bb._data.cb, callback_process, this);
#else
    pthread_mutex_lock(&_mutex);
    pthread_cond_signal(&_cond);
    pthread_mutex_unlock(&_mutex);
#endif

}
*/



#if PLATFORM_WEB

static void callback_process(char* data, int size, void* arg) {
    ByteBufferStream bb;
    bb._data.attach((uint8_t*)data, size);
    variant data_out;
    bb.readVariant(&data_out);
    bb._data.detach();
    ((Worker*)arg)->dispatchProcessResult(data_out);
}

void Worker::dispatchProcessResult(const variant& data_out) {
    auto callback = _pending.front();
    callback(data_out);
    _pending.pop_front();
}
#endif




