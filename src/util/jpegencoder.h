//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class JpegEncoder : public Object {
public:
    
    virtual void encode(class Bitmap* bitmap, std::function<void(const bytearray&)> result);
    
protected:
    
    Worker* _worker;
};
