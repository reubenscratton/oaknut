//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>
#include <fstream>

/**
 FileLocalStore
 
 File-based object storage. Records are variable length. The primary key index is kept in a separate file
 which is kept in RAM while the store is open. This index contains a SORTED list of pair<key,offset>.
 
 
 */

class FileLocalStore : public LocalStore {
public:
    
    typedef uint32_t FILEOFFSET;
    typedef struct {
        FILEOFFSET offset;
        FILEOFFSET size;
    } INDEX_ENTRY;
    typedef map<Variant,INDEX_ENTRY> INDEX;
    FILEOFFSET _deadSpace;
    
    FileLocalStore(const string& name, const string& primaryKeyName) : LocalStore(name, primaryKeyName) {
        _mainFileName = app.getDirectoryForFileType(App::FileType::General);
        if (_mainFileName.at(_mainFileName.length()-1)!='/') {
            _mainFileName.append("/");
        }
        _mainFileName.append(name);
        _indexFileName = _mainFileName;
        _mainFileName.append(".dat");
        _indexFileName.append(".idx");
    }
    
    // Open & close methods.
    virtual void open() {
        _deadSpace = 0;
        FileStream idxstrm(_indexFileName);
        if (idxstrm.openForRead()) {
            idxstrm.readBytes(sizeof(_deadSpace), &_deadSpace);
            while (idxstrm.hasMoreToRead()) {
                Variant key;
                INDEX_ENTRY entry;
                if (!key.readSelfFromStream(&idxstrm)) {
                    break;
                }
                idxstrm.readBytes(sizeof(entry), &entry);
                _index.insert(make_pair(key,entry));
            }
            idxstrm.close();
        }
    }
    virtual void close() {
        flush();
        _index.clear();
        _file.close();
    }
    
    virtual void flush() {
        if (_indexDirty) {
            FileStream idxstrm(_indexFileName);
            idxstrm.openForWrite();
            idxstrm.writeBytes(sizeof(_deadSpace), &_deadSpace);
            for (auto it : _index) {
                it.first.writeSelfToStream(&idxstrm);
                idxstrm.writeBytes(sizeof(it.second), &it.second);
            }
            idxstrm.close();
            _indexDirty = false;
        }
    }


    virtual int getCount() {
        return (int)_index.size();
    }
    virtual bool moveFirst() {
        _it = _index.begin();
        return _it != _index.end();
    }
    virtual bool find(const Variant& primaryKeyVal) {
        _it = _index.find(primaryKeyVal);
        return _it != _index.end();
    }

    VariantMap* readCurrent() {
        if (_it == _index.end()) {
            return NULL;
        }
        ByteBufferStream bbs(_it->second.size);
        openFile();
        _file.seekp(_it->second.offset);
        _file.read((char*)bbs._data.data, _it->second.size);
        assert(_file.gcount() == _it->second.size);
        _it++;
        VariantMap* map = new VariantMap();
        map->readSelfFromStream(&bbs);
        return map;
    }
    
    void remove(const Variant& primaryKeyVal) {
        auto saved_it = _it;
        if (find(primaryKeyVal)) {
            _deadSpace += _it->second.size;
            _index.erase(_it);
            _it = saved_it;
            _indexDirty = true;
        }
    }
    virtual void put(ISerializable* object) {
        VariantMap map;
        object->writeSelf(&map);
        
        Variant key = map.get(_primaryKeyName);
        assert(key.type != EMPTY); // key is mandatory! (that might change)
        _indexDirty = true;
        openFile();

        // Serialize the map to a byte array
        ByteBufferStream bbs;
        map.writeSelfToStream(&bbs);
        int cb = (int)bbs.offsetWrite;
        
        // If there's an existing object with this key...
        if (find(key)) {
            
            // If the new object is same size or smaller then we can overwrite the existing record
            int sizeChange = cb - _it->second.size;
            if (sizeChange <= 0) {
                _file.seekp(_it->second.offset);
                _file.write((char*)bbs._data.data, cb);
                //assert(written == cb);
                _it->second.size = cb;
                _deadSpace += -sizeChange;
                return;
            }
            
            // Remove the old object
            _deadSpace += _it->second.size;
            _index.erase(_it);
        }
        
        // Append new record to end of file
        _file.seekp(0, ios_base::end);
        INDEX_ENTRY entry;
        entry.offset = (FILEOFFSET)_file.tellp();
        entry.size = cb;
        auto result = _index.insert(make_pair(key, entry));
        _it = result.first;
        _file.write((char*)bbs._data.data, cb);
        //assert(written == cb);
    }

    
protected:
    void openFile() {
        if (!_file.is_open()) {
            _file.open(_mainFileName.data(), fstream::in | fstream::out | fstream::binary);
            if (!_file.is_open()) {
                _file.open(_mainFileName.data(), fstream::in | fstream::out | fstream::binary | fstream::app);
                assert(_file.is_open());
            }
        }
    }
    
protected:
    string _mainFileName;
    string _indexFileName;
    std::fstream _file;
    bool _fileOpenForWrite;
    INDEX _index;
    INDEX::iterator _it;
    bool _indexDirty;
    
    
};

LocalStore* LocalStore::create(const string& name, const string& primaryKeyName) {
    return new FileLocalStore(name, primaryKeyName);
}

LocalStore::LocalStore(const string& name, const string& primaryKeyName) {
    _primaryKeyName = primaryKeyName;
}


