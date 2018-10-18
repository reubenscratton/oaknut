//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>


class FaceDetectorWeb : public FaceDetector {
public:
    
    FaceDetectorWeb() {
        EM_ASM_({
            self.importScripts("face_detector.js");
        });
    }
    virtual int detectFaces(class Bitmap* bitmap) {
        PIXELDATA pixelData;
        bitmap->lock(&pixelData, false);

        int numFaces = EM_ASM_INT({
            var pixelsPtr = new Uint8Array(Module.HEAPU8.buffer, $0, $1);
            var nf = face_detector.detect(pixelsPtr, $2, $3, 4.0, 1.5, 2.0, 0.05);
            return nf ? nf.length : 0;
            //return face_detector.detect(pixelsPtr, $2, $3, 1.0, 1.25, 1.5, 0.2);
        }, pixelData.data, pixelData.cb, bitmap->_width, bitmap->_height);
        bitmap->unlock(&pixelData, false);
        return numFaces;
    }
};


FaceDetector* FaceDetector::create() {
    return new FaceDetectorWeb();
}

#endif
