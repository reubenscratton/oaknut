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
    
    ByteBuffer();
    ByteBuffer(size_t cb);
    ByteBuffer(uint8_t* data, size_t cb, bool copy=true);
    ByteBuffer(const ByteBuffer& data);
    ByteBuffer(const string& str);
    ~ByteBuffer();
    void set(const void* rawData, size_t len);
    void attach(uint8_t* data, size_t cb);
    void detach();
    
    static ByteBuffer* createFromFile(const string& path);
    void saveToFile(const string& path);
    
    string toString();
    
    // ISerializable
    virtual bool readSelfFromStream(Stream* stream);
    virtual bool writeSelfToStream(Stream* stream) const;
};

