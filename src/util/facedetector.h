//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class FaceDetector : public Object {
public:

    static FaceDetector* create();
    
    virtual int detectFaces(class Bitmap* bitmap) = 0;

protected:
    FaceDetector();
    
};

