//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


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
                dynCall('viii', $3, [$0, gotSet(db), $4]);
            };
            request.onupgradeneeded = function(event) {
                var db = event.target.result;
                var storeName = Pointer_stringify($1);
                var primaryKey = Pointer_stringify($2);
                var objectStore = db.createObjectStore(storeName, { keyPath: primaryKey });
            };

        }, this, // $0
            _name.c_str(), // $1
            _primaryKeyName.c_str(), // $2
            openCallbackFromJs, // $3
            new std::function<void()>(callback) // $4
        );
    }
    virtual void close() {
    }
    
    
    virtual void getCount(std::function<void(int)> success) {
        
    }
    
    static void getAllCallbackFromJs(WebLocalStore* store, int gotIndex, std::function<void(variant*)>* callback) {
        if (gotIndex >= 0) {
            variant value;
            value.fromJavascriptVal(gotIndex);
            (*callback)(&value);
        } else {
            (*callback)(NULL);
            delete callback;
        }
    }
    virtual void getAll(std::function<void(variant*)> callback) {
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
                    dynCall('viii', $2, [$0, gotSet(cursor.value), $3]);
                    cursor.continue();
                } else {
                    dynCall('viii', $2, [$0, -1, $3]);
                }
            };
        }, this, _name.c_str(), getAllCallbackFromJs, new std::function<void(variant*)>(callback), gotIndex);

    }
    virtual void getOne(const variant& primaryKeyVal, std::function<void(variant*)> success) {
        
    }
    
    
    
    static void removeCallbackFromJs(WebLocalStore* store, std::function<void(void)>* callback) {
        (*callback)();
        delete callback;
    }
    void remove(const variant& primaryKeyVal, std::function<void(void)> callback) {
        EM_ASM_ARGS({
            var name = Pointer_stringify($1);
            var db = gotGet($4);
            var tx = db.transaction(name, "readwrite");
            var store = tx.objectStore(name);
            var req = store.delete(gotGet($5));
            tx.oncomplete = function(event) {
                dynCall('vii', $2, [$0, $3]);
            };
            tx.onerror = function(event) {
            };
        }, this, // $0
            _name.c_str(), // $1
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
    virtual void put(ISerializeToVariant* object, std::function<void(void)> callback) {
        
        // Serialise object to a variant
        variant v;
        object->toVariant(v);
        const variant& key = v.get(_primaryKeyName);
        assert(!key.isEmpty()); // key is mandatory! (that might change)

        // Convert variant to a JS object
        val jsobj = v.toJavascriptVal();
        
        // Send to JS IndexedDb
        EM_ASM_ARGS({
            var name = Pointer_stringify($1);
            var db = gotGet($4);
            var obj = gotGet($5);
            var tx = db.transaction(name, "readwrite");
            tx.oncomplete = function() {
                dynCall('vii', $2, [$0, $3]);
            };
            var store = tx.objectStore(name);
            store.put(obj);
        }, this, // $0
           _name.c_str(), // $1
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
typedef map<variant,INDEX_ENTRY> INDEX;

class FileLocalStore : public LocalStore {
public:
    
    FILEOFFSET _deadSpace;
    
    FileLocalStore(const string& name, const string& primaryKeyName) : LocalStore(name, primaryKeyName) {
    }
    
    // Open & close methods.
    void open(std::function<void()> callback) override {
        _deadSpace = 0;
        Task::enqueue({
                            
            // Task 1: Open main file and load index file contents on IO thread
            {Task::IO, [=](variant&) -> variant {
                string mainFileName = _name + ".dat";
                _mainFd = ::open(mainFileName.c_str(), O_RDWR, O_CREAT);
                return Task::fileLoad(_name + ".idx");
            }},
            
            // Task 2: Process index file contents into usable form
            {Task::Background, [=](variant& indexFileContents) -> variant {
                bytestream strm(indexFileContents.bytearrayRef());
                strm.read(_deadSpace);
                while (!strm.eof()) {
                    variant key;
                    INDEX_ENTRY entry;
                    strm.read(key);
                    strm.readBytes(sizeof(entry), &entry);
                    _index.insert(make_pair(key,entry));
                }
                return variant(true);
            }},
            
            // Task 3: Run main thread callback to advertise we're ready for read/write
            {Task::MainThread, [=](variant& result) -> variant {
                callback();
                return variant();
            }}
        });
    }
    
    void close() override {
        commitChanges(true);
    }
    
    
    virtual void commitChanges(bool close) {
        vector<Task::spec> tasks;
        if (_indexDirty) {
            _indexDirty = false;
            tasks.push_back({Task::Background, [=](variant&) -> variant {
                _indexMutex.lock();
                auto indexCopy = _index;
                _indexMutex.unlock();
                bytestream strm;
                strm.writeBytes(sizeof(_deadSpace), &_deadSpace);
                for (auto it : indexCopy) {
                    strm.write(it.first);
                    strm.write(it.second);
                }
                return variant(strm);
            }});
            tasks.push_back({Task::IO, [=](variant& indexFileContents) -> variant {
                return Task::fileSave(_name + ".idx", indexFileContents.bytearrayRef());
            }});
        };
        if (close) {
            auto fd = _mainFd;
            _mainFd = 0;
            tasks.push_back({Task::IO, [=](variant&) -> variant {
                ::close(fd);
                return variant();
            }});
            tasks.push_back({Task::MainThread, [=](variant&) -> variant {
                _index.clear();
                return variant();
            }});
        }
        Task::enqueue(tasks);
    }

    void getCount(std::function<void(int)> success) override {
        success((int)_index.size());
    }
    
    void getAll(std::function<void(variant*)> callback) override {
        vector<Task::spec> tasks;
        auto it = _index.begin();
        while (it != _index.end()) {
            auto index_entry = it->second;
            tasks.push_back(
                {Task::IO, [=](variant&) -> variant {
                    if (-1 == ::lseek(_mainFd, index_entry.offset, SEEK_SET)) {
                        return variant(error::fromErrno());
                    }
                    bytearray bytes(index_entry.size);
                    ssize_t sz = ::read(_mainFd, bytes.data(), index_entry.size);
                    if (sz == -1) {
                        return variant(error::fromErrno());
                    }
                    assert(index_entry.size == sz);
                    return variant(bytes);
                }
            });
            tasks.push_back(
                {Task::MainThread, [=](variant& result) -> variant {
                    callback(&result);
                    return variant();
                }
            });
            it++;
        }
        tasks.push_back(
            {Task::MainThread, [=](variant&) -> variant {
                callback(NULL);
                return variant();
            }
        });
        Task::enqueue(tasks);
    }
    
    void getOne(const variant& primaryKeyVal, std::function<void(variant*)> callback) override {
        assert(callback);
        auto it = _index.find(primaryKeyVal);
        if (it == _index.end()) {
            callback(NULL);
            return;
        }
        auto index_entry = it->second;
        Task::enqueue({
            {Task::IO, [=](variant&) -> variant {
                if (-1 == ::lseek(_mainFd, index_entry.offset, SEEK_SET)) {
                    return variant(error::fromErrno());
                }
                bytearray bytes(index_entry.size);
                ssize_t sz = ::read(_mainFd, bytes.data(), index_entry.size);
                if (sz == -1) {
                    return variant(error::fromErrno());
                }
                assert(index_entry.size == sz);
                return variant(bytes);
            }},
            {Task::MainThread, [=](variant& result) -> variant {
                callback(&result);
                return variant();
            }},

        });
    }

    void remove(const variant& primaryKeyVal, std::function<void()> callback) override {
        auto it = _index.find(primaryKeyVal);
        if (it != _index.end()) {
            _deadSpace += it->second.size;
            _indexMutex.lock();
            _index.erase(it);
            _indexMutex.unlock();
            _indexDirty = true;
            commitChanges(false);
        }
        callback();
    }
    
    void put(ISerializeToVariant* object, std::function<void(void)> callback) override {
        variant v;
        object->toVariant(v);
        
        auto key = v.get(_primaryKeyName);
        assert(!key.isEmpty()); // key is mandatory! (that might change)
        _indexDirty = true;

        // Serialize the map to a byte array
        bytestream strm;
        strm.write(v);
        int cb = (int)strm.offsetWrite;
        
        // See if there's an existing object with this key...
        auto it = _index.find(key);
        if (it != _index.end()) {
            
            // If the new object is same size or smaller then we can overwrite the existing record
            int sizeChange = cb - it->second.size;
            if (sizeChange <= 0) {
                Task::enqueue({
                    {Task::IO, [=](variant&) -> variant {
                        if (-1 == ::lseek(_mainFd, it->second.offset, SEEK_SET)) {
                            return error::fromErrno();
                        }
                        ssize_t sz = ::write(_mainFd, strm.data(), strm.size());
                        if (sz == -1) {
                            return error::fromErrno();
                        }
                        return variant(strm.size() == sz);
                    }},
                    {Task::MainThread, [=](variant&) -> variant {
                        callback();
                        return variant();
                    }}
                });
                it->second.size = cb;
                _deadSpace += -sizeChange;
                return;
            }
            
            // Remove the old object
            _deadSpace += it->second.size;
            _indexMutex.lock();
            _index.erase(it);
            _indexMutex.unlock();
        }
        
        // Append new record to end of file
        Task::enqueue({
            {Task::IO, [=](variant&) -> variant {
                if (-1 == ::lseek(_mainFd, 0, SEEK_END)) {
                    return error::fromErrno();
                }
                ssize_t sz = ::write(_mainFd, strm.data(), strm.size());
                if (sz == -1) {
                    return error::fromErrno();
                }
                return variant(strm.size() == sz);
            }},
            {Task::MainThread, [=](variant& result) -> variant {
                if (!result.isError()) {
                    INDEX_ENTRY entry;
                    entry.offset = result.intVal();
                    entry.size = cb;
                    _indexMutex.lock();
                    _index.insert(make_pair(key, entry));
                    _indexMutex.unlock();
                }
                callback();
                return variant();
            }}
        });
        
    }

    
protected:
    int _mainFd;
    bool _isOpening;
    bool _fileOpenForWrite;
    INDEX _index;
    std::mutex _indexMutex;
    std::atomic<bool> _indexDirty;
    
    
};

LocalStore* LocalStore::create(const string& name, const string& primaryKeyName) {
    return new FileLocalStore(name, primaryKeyName);
}
#endif

LocalStore::LocalStore(const string& name, const string& primaryKeyName) {
    _name = name;
    _primaryKeyName = primaryKeyName;
}


