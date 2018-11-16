//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

#if !BUILD_AS_WORKER

#if 0
class FaceDetectorWorker : public WorkerWeb {
public:
    FaceDetectorWorker() : WorkerWeb("facedetect") {
    }
};
#else
class FaceDetectorWorker : public Worker {
public:
    val _worker;
    list<std::function<void(const variant&)>> _pending;
    
    static void thunk(FaceDetectorWorker* fdw, int numFaces) {
        variant data_out;
        data_out.setType(variant::MAP);
        data_out.set("numFaces", numFaces);
        ByteBufferStream bbOut;
        bbOut.writeVariant(data_out);
        auto callback = fdw->_pending.front();
        callback(data_out);
        fdw->_pending.pop_front();
    }
    FaceDetectorWorker() : _worker(val::null()) {
    }
    void start(const variant& config) override {
        int gotIndex = EM_ASM_INT({
            var worker = new Worker('face_detector.js');
            worker.onmessage = function(e) {
                var result = e.data;
                Runtime.dynCall("vii", $1, [$0, result]);
            };
            return gotSet(worker);
        }, this, thunk);
        _worker = val::global("gotGet")(gotIndex);
    }
    
    void process(const variant& data_in, std::function<void(const variant&)> callback) override {
        _pending.push_back(callback);
        val jsdata_in = data_in.toJavascriptVal();
        _worker.call<void>("postMessage", jsdata_in);
        /*
         var nf = face_detector.detect(pixelsPtr, $2, $3, 4.0, 1.5, 2.0, 0.05);
         //var nf = face_detector.detect(pixelsPtr, $2, $3, 1.0, 1.25, 1.5, 0.2);
         */
    }
    void stop(std::function<void()> onStop) override {
        
    }
    
};
#endif

DECLARE_DYNCREATE(FaceDetectorWorker);

#else

extern "C" {
    

void ww_start(char* data, int size) {
    EM_ASM_({
        self.importScripts("face_detector.js");
    });
    emscripten_worker_respond(0, 0);
}
void ww_process(char* bytes, int size) {
    ByteBufferStream bb(new ByteBuffer((uint8_t*)bytes,size));
    variant data;
    bb.readVariant(&data);
    int width = data.intVal("width");
    int height = data.intVal("height");
    const bytearray& pixels = data.bytearrayVal("data");


    int numFaces = EM_ASM_INT({
        var pixelsPtr = new Uint8Array(HEAPU8.buffer, $0, $1);
        var nf = face_detector.detect(pixelsPtr, $2, $3, 4.0, 1.5, 2.0, 0.05);
        //var nf = face_detector.detect(pixelsPtr, $2, $3, 1.0, 1.25, 1.5, 0.2);
        return nf ? nf.length : 0;
    }, pixels.data(), pixels.size(), width, height);

    variant data_out;
    data_out.setType(variant::MAP);
    data_out.set("numFaces", numFaces);
    ByteBufferStream bbOut;
    bbOut.writeVariant(data_out);
    emscripten_worker_respond((char*)bbOut._data.data, bbOut._data.cb);
}
void ww_stop(char* data, int size) {
    emscripten_worker_respond(0, 0);
}

}

#endif

#endif
