//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

FaceDetector::FaceDetector() {
    _worker = (Worker*)Object::createByName("FaceDetectorWorker");
    assert(_worker);
    _worker->start(variant());
}

bool FaceDetector::isBusy() {
    return _isBusy;
}

void FaceDetector::detectFaces(class Bitmap* bitmap, std::function<void(int)> result) {
    _isBusy = true;
    variant data_in;
    data_in.set("width", bitmap->_width);
    data_in.set("height", bitmap->_height);
    PIXELDATA pixelData;
    bitmap->lock(&pixelData, false);
    bytearray bytes((uint8_t*)pixelData.data, pixelData.cb);
    
    data_in.set("data", bytes);
    bitmap->unlock(&pixelData, false);
    _worker->process(data_in, [=](const variant& data_out) {
        int numFaces = data_out.intVal("numFaces");
        result(numFaces);
        _isBusy = false;
    });
}

DECLARE_DYNCREATE(FaceDetector);

