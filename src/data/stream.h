//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 Abstract base class for a thing that can read and write elementary data
 */
class Stream : public Object {
public:
    size_t offsetRead;
    size_t offsetWrite;
    
    Stream();
        
    virtual bool hasMoreToRead() = 0;
    virtual bool writeBytes(size_t cb, const void* bytes) = 0;
    virtual bool readBytes(size_t cb, void* bytes) = 0;
    
    bool readInt32(int32_t* val);
    bool writeInt32(int32_t val);
    bool readUint32(uint32_t* val);
    bool writeUint32(uint32_t val);
    bool readString(string* str);
    bool writeString(const string& str);
};
