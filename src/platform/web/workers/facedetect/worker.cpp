//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
/*
#if PLATFORM_WEB

#include <oaknut.h>

#if !BUILD_AS_WORKER

#if !USE_CPP_WORKER
class FaceDetectorWorker : public WorkerImpl {
public:
    val _worker;
    list<std::function<void(const variant&)>> _pending;
    
    static void thunk(FaceDetectorWorker* fdw, int gotIndex) {
        variant data_out;
        data_out.fromJavascriptVal(gotIndex);
        auto callback = fdw->_pending.front();
        callback(data_out);
        fdw->_pending.pop_front();
    }
    FaceDetectorWorker() : WorkerImpl(), _worker(val::null()) {
    }
    void start_(const variant& config) override {
        int gotIndex = EM_ASM_INT({
            var worker = new Worker('face_detector.js');
            worker.onmessage = function(e) {
                var result = e.data;
                Runtime.dynCall("vii", $1, [$0, gotSet(result)]);
            };
            return gotSet(worker);
        }, this, thunk);
        _worker = val::global("gotGet")(gotIndex);
    }
    
    void process(const variant& data_in, std::function<void(const variant&)> callback) override {
        _pending.push_back(callback);
        val jsdata_in = data_in.toJavascriptVal();
        _worker.call<void>("postMessage", jsdata_in);
    }
    void stop(std::function<void()> onStop) override {
        
    }
    
};

DECLARE_DYNCREATE(FaceDetectorWorker);

#endif

#else



#endif

#endif
*/

