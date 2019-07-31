//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup base_group
 * @class Object
 * @brief Base class for all reference-counted types.
 */
class Object {
public:
	int _refs;

	Object();
	virtual ~Object();
    
    /**
     Increments the internal reference counter. NB: Not threadsafe, cos we don't have threads.
     */
	void retain();
    
    /**
     Decrements the internal reference counter. If the counter reaches zero the object is moved
     to a queue of objects that will be free()d between frames. NB: Not threadsafe.
     */
	void release();
	
	void* operator new(size_t sz);
    
#ifdef DEBUG
    virtual string debugDescription();
#endif
    
    static void flushAutodeletePool();
    
    static std::map<string, Object* (*)()>* s_classRegister;

    template<typename T, class ...ARGS>
    class Registrar {
    private: static Object* createT(ARGS... args) {return new T(args...); }
    public:
        Registrar(const string& className) {
            if (!s_classRegister) {
                s_classRegister = new std::map<string, Object*(*)()>();
            }
            s_classRegister->insert(std::make_pair(className, reinterpret_cast<Object*(*)()>(&createT)));
        }
    };


    template<class ...ARGS>
    static Object* createByName(const string& className, ARGS... args) {
        const auto& constructor = s_classRegister->find(className);
        assert(constructor != s_classRegister->end()); // oops!
        auto real_constructor = reinterpret_cast<Object*(*)(ARGS...)>(constructor->second);
        return real_constructor(args...);
    }

    template<class ReturnType, class... Ts>
    ReturnType callFunction(void *function, const Ts&... args) {
    }
};

/**
 Declare a class as being dynamically creatable with Object::createByName("className",...).
 The class must have a public constructor that takes the same argument types as listed after
 the class name.
 */
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define VA_ARGS(...) , ##__VA_ARGS__
#define DECLARE_DYNCREATE(X, ...) static Object::Registrar<X VA_ARGS(__VA_ARGS__)> s_classReg##X(STRINGIFY(X))


#if DEBUG
extern void* DBGOBJ;
#endif

/**
 * @ingroup base_group
 * @class sp<T>
 * @brief A smart pointer class that holds a strong reference to an Object-derived type.
 */
template<class T>
class sp {
public:
	T* _obj;
    
    template<class TC>
    TC* as() const {
        return static_cast<TC*>(_obj);
    }
/*#if DEBUG
    void dbgLog(bool retain) {
        if (_obj && _obj==DBGOBJ) {
            app->log("DBGOBJ:%s=%d", retain?"+1":"-1", _obj->_refs);
        }
    }
#else*/
#define dbgLog(x)
//#endif

    sp() : _obj(NULL) {
    }
    sp(T* obj) : _obj(obj) {
		if (obj) {
			obj->retain();
            dbgLog(true);
		}
    }
    sp(const sp<T>& sp) : _obj(sp._obj) { // Copy constructor
		if (_obj) {
			_obj->retain();
            dbgLog(true);
		}
    }
    ~sp() {
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
    T* operator& () const {
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
    sp<T>& operator= (const sp<T>& obj) {
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





