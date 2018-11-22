//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

FaceDetector::FaceDetector() : Worker("FaceDetectorWorker")
{
    start(variant());
}

bool FaceDetector::isBusy() {
    return _isBusy;
}

void FaceDetector::detectFaces(class Bitmap* bitmap, const RECT& roiRect, std::function<void(vector<RECT>)> resultCallback) {
    _isBusy = true;
    variant data_in;
    data_in.set("width", bitmap->_width);
    data_in.set("height", bitmap->_height);
    data_in.set("roiLeft", roiRect.origin.x);
    data_in.set("roiTop", roiRect.origin.y);
    data_in.set("roiWidth", roiRect.size.width);
    data_in.set("roiHeight", roiRect.size.height);
    PIXELDATA pixelData;
    bitmap->lock(&pixelData, false);
    bytearray bytes((uint8_t*)pixelData.data, pixelData.cb);
    
    data_in.set("data", bytes);
    bitmap->unlock(&pixelData, false);
    process(data_in, [=](const variant& data_out) {
        vector<RECT> results;
        auto& vresults = data_out.arrayVal();
        for (auto& vrect : vresults) {
            RECT rect;
            rect.origin.x = vrect.floatVal("x");
            rect.origin.y = vrect.floatVal("y");
            rect.size.width = vrect.floatVal("width");
            rect.size.height = vrect.floatVal("height");
            results.push_back(rect);
        }
        //app.log("Found %d faces!", results.size());
        resultCallback(results);
        _isBusy = false;
    });
}
