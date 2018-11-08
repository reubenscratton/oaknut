//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class FileStream : public Stream {
public:
    FileStream(string path);
    ~FileStream();
    bool openForRead();
    bool openForWrite();
    void close();
    
    bool hasMoreToRead() override;
    bool writeBytes(size_t cb, const void* bytes) override;
    bool readBytes(size_t cb, void* bytes) override;
    void setWriteOffset(size_t offset) override;
    
protected:
    string _path;
    FILE* _file;
    
};
