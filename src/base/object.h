//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class Object {
public:
	int _refs;

	Object();
	virtual ~Object();
	void retain();
	void release();
	
	void* operator new(size_t sz);
    
#ifdef DEBUG
    virtual string debugDescription();
#endif
    
    static void flushAutodeletePool();
};

#if DEBUG
extern void* DBGOBJ;
#endif

template<class T>
class ObjPtr {
public:
	T* _obj;
    
/*#if DEBUG
    void dbgLog(bool retain) {
        if (_obj && _obj==DBGOBJ) {
            app.log("DBGOBJ:%s=%d", retain?"+1":"-1", _obj->_refs);
        }
    }
#else*/
#define dbgLog(x)
//#endif

    ObjPtr() : _obj(NULL) {
    }
    ObjPtr(T* obj) : _obj(obj) {
		if (obj) {
			obj->retain();
            dbgLog(true);
		}
    }
    ObjPtr(const ObjPtr<T>& sp) : _obj(sp._obj) { // Copy constructor
		if (_obj) {
			_obj->retain();
            dbgLog(true);
		}
    }
    ~ObjPtr() {
		if (_obj) {
			_obj->release();
            dbgLog(false);
		}
    }
    T& operator* () {
        return *(T*)_obj;
    }
    
    T* operator-> () const {
        return (T*)_obj;
    }
	void assign(T* obj) {
		if (obj != _obj) {
			if (_obj) {
				_obj->release();
                dbgLog(false);
			}
			_obj = obj;
			if (_obj) {
				_obj->retain();
                dbgLog(true);
			}
		}
	}
    T* operator= (T* obj) {
		assign(obj);
        return (T*)_obj;
    }
    ObjPtr<T>& operator= (const ObjPtr<T>& obj) {
		assign(obj._obj);
        return *this;
    }
    
    operator T*() const {
        return (T*)_obj;
    }
    bool isSet() {
        return _obj!=NULL;
    }
    
};

