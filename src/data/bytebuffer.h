//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup data_group
 * @class ByteBuffer
 * @brief Simple wrapper for a refcounted contiguous array of bytes, analogous
 *        to NSData on iOS or byte[] in Java.
 */
class ByteBuffer : public Object, public ISerializable {
public:
    uint8_t* data;
    size_t cb;
    bool _owns;
    
    ByteBuffer();
    ByteBuffer(size_t cb);
    ByteBuffer(uint8_t* data, size_t cb, bool copy=true, bool owns=true);
    ByteBuffer(const ByteBuffer& data);
    ByteBuffer(const string& str);
    ~ByteBuffer();
    
    static ByteBuffer* createFromFile(const string& path);
    void saveToFile(const string& path);
    
    string toString(bool copy);
    
    // ISerializable
    virtual bool readSelfFromStream(Stream* stream);
    virtual bool writeSelfToStream(Stream* stream) const;
};

