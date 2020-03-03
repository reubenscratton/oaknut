//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


/**
 * @ingroup data_group
 * @class bytestream
 * @brief A dynamic bytearray that can be read from or written to.
 */

class bytestream : public bytearray {
public:

    bytestream();
    bytestream(int cb);
    bytestream(bytearray& src);
    
    bytearray getWrittenBytes();
    
    bool eof();
    template <typename T>
    bool read(T& val) { return readBytes(sizeof(T), &val); }
    template <typename T>
    bool write(const T& val) { return writeBytes(sizeof(T), &val); }
    
    template <typename K, typename V>
    bool read(map<K,V>& val) {
        size_t c=0;
        if (!read(c)) {
            return false;
        }
        for (int i=0 ; i<c ; i++) {
            pair<K,V> e;
            if (!read(e.first)) {
                return false;
            }
            if (!read(e.second)) {
                return false;
            }
            val.insert(e);
        }
        return true;
    }

    template <typename K, typename V>
    bool write(const map<K,V>& val) {
        if (!write(val.size())) {
            return false;
        }
        for (auto& it: val) {
            if (!write(it.first)) {
                return false;
            }
            if (!write(it.second)) {
                return false;
            }
        }
        return true;
    }

    bool writeBytes(size_t cb, const void* bytes);
    bool readBytes(size_t cb, void* bytes);
    void setWriteOffset(size_t offset);
    
    size_t offsetRead;
    size_t offsetWrite;
    
    bool hasMoreToRead();
};

