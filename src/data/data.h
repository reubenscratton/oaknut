//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class Stream : public Object {
public:
    size_t offsetRead;
    size_t offsetWrite;
    
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

class ISerializable {
public:
    virtual bool readSelfFromStream(Stream* stream) =0;
    virtual bool writeSelfToStream(Stream* stream) =0;
};

class Data : public Object, ISerializable {
public:
    uint8_t* data;
    size_t cb;
    
    Data();
    Data(size_t cb);
    Data(const Data& data);
    Data(const string& str);
    ~Data();
    
    static Data* createFromFile(const string& path);
    void saveToFile(const string& path);

    // ISerializable
    virtual bool readSelfFromStream(Stream* stream);
    virtual bool writeSelfToStream(Stream* stream);

};



/**
 Variant encapsulates all types of plain old data (i.e. not Objects).
 It's serialized form is efficient and platform-independent, it's runtime form is not.
 It is only intended to be used during serialization (i.e. to support KeyValueMap).
 It is expected that most classes will serialize via KeyValueMap.
 */
typedef enum {
    EMPTY,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT,
    DOUBLE,
    DATA,
    MAP
} VariantType;


class KeyValueMap : public Object, ISerializable {
public:
    
    bool hasValue(const string& key) const;
    int getInt(const string& key) const;
    uint64_t getUint64(const string& key) const;
    KeyValueMap* getMap(const string& key) const;
    float getFloat(const string& key) const;
    void setFloat(const string& key, float val);
    void setInt(const string& key, int val);
    void setUint64(const string& key, uint64_t val);
    void setMap(const string& key, KeyValueMap* val);
    string getString(const string& key) const;
    void setString(const string& key, const string& val);
    Data* getData(const string& key) const;
    void setData(const string& key, const Data* val);

    class Variant : public ISerializable {
    public:
        VariantType type;
        union {
            int8_t i8;
            int16_t i16;
            int32_t i32;
            int64_t i64;
            uint8_t u8;
            uint16_t u16;
            uint32_t u32;
            uint64_t u64;
            float f;
            double d;
            ObjPtr<Data> data;
            ObjPtr<KeyValueMap> map;
        };
        Variant();
        Variant(const Variant& var);
        Variant& operator=(const Variant& rhs);
        ~Variant();
        
        // ISerializable
        virtual bool readSelfFromStream(class Stream* stream);
        virtual bool writeSelfToStream(Stream* stream);
        
    private:
        inline void assign(const Variant& src);
    };
    map<string, Variant> _map;

    
    // ISerializable
    virtual bool readSelfFromStream(class Stream* stream);
    virtual bool writeSelfToStream(Stream* stream);
};

class ISerializableKeyed {
public:
    virtual void readSelfFromKeyValueMap(const KeyValueMap* map) =0;
    virtual void writeSelfToKeyValueMap(KeyValueMap* map) =0;
};


class DataStream : public Stream {
public:
    Data _data;
    
    virtual bool hasMoreToRead();
    virtual bool writeBytes(size_t cb, const void* bytes);
    virtual bool readBytes(size_t cb, void* bytes);
};

class FileStream : public Stream {
public:
    FileStream(string path);
    ~FileStream();
    bool openForRead();
    bool openForWrite();
    void close();
    
    virtual bool hasMoreToRead();
    virtual bool writeBytes(size_t cb, const void* bytes);
    virtual bool readBytes(size_t cb, void* bytes);
    
protected:
    string _path;
    FILE* _file;
    
};

typedef std::function<void(class Data*)> OnDataLoadCompleteDelegate;

