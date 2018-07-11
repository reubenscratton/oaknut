//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class ByteBufferStream : public Stream {
public:
    ByteBuffer _data;
    
    ByteBufferStream();
    ByteBufferStream(int cb);
    ByteBufferStream(ByteBuffer* data);
    
    virtual bool hasMoreToRead();
    virtual bool writeBytes(size_t cb, const void* bytes);
    virtual bool readBytes(size_t cb, void* bytes);
};

