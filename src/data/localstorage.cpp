//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>
#include <fstream>

#ifdef PLATFORM_WEB



class WebLocalStore : public LocalStore {
public:
    
    val _db;
    
    WebLocalStore(const string& name, const string& primaryKeyName) : LocalStore(name, primaryKeyName), _db(val::null()) {
    }
    
    
    // Open & close methods.
    static void openCallbackFromJs(WebLocalStore* store, int gotIndex, std::function<void()>* openCallback) {
        store->_db = val::global("gotGet")(gotIndex);
        (*openCallback)();
        delete openCallback;
    }
    virtual void open(std::function<void()> callback) {
        EM_ASM_ARGS({
            var request = window.indexedDB.open("OaknutApp", 1);
            request.onerror = function(event) {
                alert("IndexedDB required!");
            };
            request.onsuccess = function(event) {
                var db = event.target.result;
                Runtime.dynCall('viii', $3, [$0, gotSet(db), $4]);
            };
            request.onupgradeneeded = function(event) {
                var db = event.target.result;
                var storeName = Pointer_stringify($1);
                var primaryKey = Pointer_stringify($2);
                var objectStore = db.createObjectStore(storeName, { keyPath: primaryKey });
            };

        }, this, // $0
            _name.data(), // $1
            _primaryKeyName.data(), // $2
            openCallbackFromJs, // $3
            new std::function<void()>(callback) // $4
        );
    }
    virtual void close() {
    }
    
    virtual void flush() {
    }
    
    
    virtual void getCount(std::function<void(int)> success) {
        
    }
    
    static void getAllCallbackFromJs(WebLocalStore* store, uint8_t* byteArray, int cb, std::function<void(VariantMap*)>* callback) {
        if (cb >= 0) {
            ByteBuffer bb(byteArray, cb, false, true);
            ByteBufferStream bbs(&bb);
            VariantMap value;
            value.readSelfFromStream(&bbs);
            (*callback)(&value);
        } else {
            (*callback)(NULL);
            delete callback;
        }
    }
    virtual void getAll(std::function<void(VariantMap*)> callback) {
        int gotIndex = val::global("gotSet")(_db).as<int>();
        EM_ASM_ARGS({
            var name = Pointer_stringify($1);
            var db = gotGet($4);
            var tx = db.transaction(name, "readonly");
            var store = tx.objectStore(name);
            var req = store.openCursor();
            req.onsuccess = function(event) {
                var cursor = event.target.result;
                if (cursor) {
                    var byteArray = cursor.value["_b"]; // UInt8Array
                    var cb = byteArray.length;
                    var dataPtr = Module._malloc(cb);
                    var heapArray = new Uint8Array(HEAPU8.buffer, dataPtr, cb);
                    heapArray.set(byteArray);
                    Runtime.dynCall('viiii', $2, [$0, dataPtr, cb, $3]);
                    cursor.continue();
                } else {
                    var dataPtr = null;//Module._malloc(4);
                    Runtime.dynCall('viiii', $2, [$0, dataPtr, -1, $3]);
                }
            };
        }, this, _name.data(), getAllCallbackFromJs, new std::function<void(VariantMap*)>(callback), gotIndex);

    }
    virtual void getOne(const Variant& primaryKeyVal, std::function<void(VariantMap*)> success) {
        
    }
    
    
    
    static void removeCallbackFromJs(WebLocalStore* store, std::function<void(void)>* callback) {
        (*callback)();
        delete callback;
    }
    void remove(const Variant& primaryKeyVal, std::function<void(void)> callback) {
        EM_ASM_ARGS({
            var name = Pointer_stringify($1);
            var db = gotGet($4);
            var tx = db.transaction(name, "readwrite");
            var store = tx.objectStore(name);
            var req = store.delete(gotGet($5));
            tx.oncomplete = function(event) {
                Runtime.dynCall('vii', $2, [$0, $3]);
            };
            tx.onerror = function(event) {
            };
        }, this, // $0
            _name.data(), // $1
            removeCallbackFromJs, // $2
            new std::function<void()>(callback), // $3
            val::global("gotSet")(_db).as<int>(), // $4
            val::global("gotSet")(primaryKeyVal.toJavascriptVal()).as<int>() // $5
        );

    }
    
    
    static void putCallbackFromJs(WebLocalStore* store, std::function<void(void)>* callback) {
        (*callback)();
        delete callback;
    }
    virtual void put(ISerializeToVariantMap* object, std::function<void(void)> callback) {
        // Serialise object to a map
        VariantMap map;
        object->writeSelfToVariantMap(map);
        Variant key = map.get(_primaryKeyName);
        assert(key.type != Variant::EMPTY); // key is mandatory! (that might change)

        /*val jsobj = val::global("Object").new_();
        for (auto it: map._map) {
            string name = it->first;
            Variant& value = it->second;
            switch (value.type) {
            }
            jsobj.set
        }*/

        // Serialise the map to a byte buffer
        ByteBufferStream bbs;
        map.writeSelfToStream(&bbs);

        // Create a JS object holding the serialized data in a byte array
        val jsobj = val::global("Object").new_();
        jsobj.set(_primaryKeyName, key.toJavascriptVal());
        val raw(typed_memory_view(bbs._data.cb, bbs._data.data));
        val buff = val::global("Uint8Array").new_(bbs._data.cb);
        buff.call<void>("set", raw);
        
        //jsobj.set("_b", raw);
        jsobj.set("_b", buff);

        // Send to JS IndexedDb
        EM_ASM_ARGS({
            var name = Pointer_stringify($1);
            var db = gotGet($4);
            var obj = gotGet($5);
            var tx = db.transaction(name, "readwrite");
            tx.oncomplete = function() {
                Runtime.dynCall('vii', $2, [$0, $3]);
            };
            var store = tx.objectStore(name);
            store.put(obj);
        }, this, // $0
           _name.data(), // $1
           putCallbackFromJs, // $2
           new std::function<void(void)>(callback), // $3
           val::global("gotSet")(_db).as<int>(), // $4
           val::global("gotSet")(jsobj).as<int>() // $5
        );
        
    }
    
    
protected:
    
    
};



LocalStore* LocalStore::create(const string& name, const string& primaryKeyName) {
    return new WebLocalStore(name, primaryKeyName);
}

#else

/**
 FileLocalStore
 
 File-based object storage. Records are variable length. The primary key index is kept in a separate file
 which is kept in RAM while the store is open. This index contains a SORTED list of pair<key,offset>.
 
 
 */

typedef uint32_t FILEOFFSET;
typedef struct {
    FILEOFFSET offset;
    FILEOFFSET size;
} INDEX_ENTRY;
typedef map<Variant,INDEX_ENTRY> INDEX;

class FileLocalStore : public LocalStore {
public:
    
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
    virtual void open(std::function<void()> callback) {
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
        callback();
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

    virtual void getCount(std::function<void(int)> success) {
        success((int)_index.size());
    }
    virtual void getAll(std::function<void(VariantMap*)> callback) {
        auto it = _index.begin();
        while (it != _index.end()) {
            ObjPtr<VariantMap> item = readItem(it->second);
            callback(item);
            it++;
        }
        callback(NULL);
    }
    virtual void getOne(const Variant& primaryKeyVal, std::function<void(VariantMap*)> callback) {
        auto it = _index.find(primaryKeyVal);
        if (it != _index.end()) {
            ObjPtr<VariantMap> item = readItem(it->second);
            callback(item);
        }
        callback(NULL);
    }

    VariantMap* readItem(const INDEX_ENTRY& indexEntry) {
        ByteBufferStream bbs(indexEntry.size);
        openFile();
        _file.seekp(indexEntry.offset);
        _file.read((char*)bbs._data.data, indexEntry.size);
        assert(_file.gcount() == indexEntry.size);
        VariantMap* map = new VariantMap();
        map->readSelfFromStream(&bbs);
        return map;
    }


    
    void remove(const Variant& primaryKeyVal, std::function<void()> callback) {
        auto it = _index.find(primaryKeyVal);
        if (it != _index.end()) {
            _deadSpace += it->second.size;
            _index.erase(it);
            _indexDirty = true;
            flush();
        }
        callback();
    }
    virtual void put(ISerializeToVariantMap* object, std::function<void(void)> callback) {
        VariantMap map;
        object->writeSelfToVariantMap(map);
        
        Variant key = map.get(_primaryKeyName);
        assert(key.type != Variant::EMPTY); // key is mandatory! (that might change)
        _indexDirty = true;
        openFile();

        // Serialize the map to a byte array
        ByteBufferStream bbs;
        map.writeSelfToStream(&bbs);
        int cb = (int)bbs.offsetWrite;
        
        // If there's an existing object with this key...
        auto it = _index.find(key);
        if (it != _index.end()) {
            
            // If the new object is same size or smaller then we can overwrite the existing record
            int sizeChange = cb - it->second.size;
            if (sizeChange <= 0) {
                _file.seekp(it->second.offset);
                _file.write((char*)bbs._data.data, cb);
                //assert(written == cb);
                it->second.size = cb;
                _deadSpace += -sizeChange;
                return;
            }
            
            // Remove the old object
            _deadSpace += it->second.size;
            _index.erase(it);
        }
        
        // Append new record to end of file
        _file.seekp(0, ios_base::end);
        INDEX_ENTRY entry;
        entry.offset = (FILEOFFSET)_file.tellp();
        entry.size = cb;
        auto result = _index.insert(make_pair(key, entry));
        //_it = result.first;
        _file.write((char*)bbs._data.data, cb);
        //assert(written == cb);
        
        callback();
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
    bool _indexDirty;
    
    
};

LocalStore* LocalStore::create(const string& name, const string& primaryKeyName) {
    return new FileLocalStore(name, primaryKeyName);
}
#endif

LocalStore::LocalStore(const string& name, const string& primaryKeyName) {
    _name = name;
    _primaryKeyName = primaryKeyName;
}


