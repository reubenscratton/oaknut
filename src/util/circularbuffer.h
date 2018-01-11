//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class CircularBuffer : public Object {
public:
    uint8_t* _bytes;
    uint32_t _length;
    uint32_t _write;
    uint32_t _read;
    uint32_t _fill;
    
    CircularBuffer(uint32_t length);
    void write(uint8_t* bytes, uint32_t cb);
    uint32_t read(uint8_t* bytes, uint32_t cb);
};
