//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class FaceDetector : public Worker {
public:

    FaceDetector();

    virtual void detectFaces(class Bitmap* bitmap, const RECT& roiRect, std::function<void(vector<RECT>)> result);

};

