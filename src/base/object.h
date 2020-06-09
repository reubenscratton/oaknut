//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#define WANT_ATOMIC_REFCOUNTS 1

/**
 * @ingroup base_group
 * @class Object
 * @brief Base class for all reference-counted types.
 */
class Object {
public:
#if WANT_ATOMIC_REFCOUNTS
    std::atomic<int> _refs;
#else
	int _refs; // intentionally not std::atomic
#if DEBUG
    std::thread::id _owningThreadId;
#endif
#endif
	Object();

    virtual ~Object();
    
    /**
     Increments the internal reference counter. Must only be called on the owning thread, i.e.
     the thread that calls the first retain().
     */
	void retain();
    
    /**
     Decrements the internal reference counter. If the counter reaches zero the object is moved
     to a thread-local queue of objects to be free()d.
     */
	void release();
	
	void* operator new(size_t sz);
    
    /**
     Return an estimated cost (in bytes) of this object in memory. Should be inclusive, i.e. include the cost of all sub objects.
     */
    virtual uint32_t getRamCost() const;
    
#ifdef DEBUG
    virtual string debugDescription();
#endif
    
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
    
    friend class Task;
};

/**
 Declare a class as being dynamically creatable with Object::createByName("className",...).
 The class must have a public constructor that takes the same argument types as listed after
 the class name.
 */
#define STRINGIFY_(x) #x##_S
#define STRINGIFY(x) STRINGIFY_(x)
#define VA_ARGS(...) , ##__VA_ARGS__
#define DECLARE_DYNCREATE(X, ...) static Object::Registrar<X VA_ARGS(__VA_ARGS__)> s_classReg##X(STRINGIFY(X))


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

    sp() : _obj(NULL) {
    }
    sp(T* obj) : _obj(obj) {
		if (obj) {
			obj->retain();
		}
    }
    sp(const sp<T>& sp) : _obj(sp._obj) { // Copy constructor
		if (_obj) {
			_obj->retain();
		}
    }
    ~sp() {
		if (_obj) {
			_obj->release();
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
			}
			_obj = obj;
			if (_obj) {
				_obj->retain();
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

/**
 sp_threadsafe lets other threads safely access an Object owned by a different thread.
 thread, then pass it to another thread where it can safely be used as if it were an sp<T>. When the SharedObject
 is destroyed it dispatches a callback to the owning thread that releases the reference.
 */
/*
template <class T>
class sp_threadsafe {
public:
    static_assert(std::is_base_of<Object, T>::value, "type must be Object-derived");

    sp_threadsafe() : _obj(nullptr), _thread(nullptr) {
    }
    sp_threadsafe(T* obj) : _obj(obj) {
        obj->retain();
        _thread = Thread::current();
    }
    sp_threadsafe(sp_threadsafe&& rhs) {
       _obj = rhs._obj;
       rhs._obj = nullptr;
       _thread = Thread::current();
       assert(_thread == rhs._thread);
    }
    sp_threadsafe& operator=(sp_threadsafe&& rhs) {
        _obj = rhs._obj;
        rhs._obj = nullptr;
        _thread = Thread::current();
        //assert(_thread == rhs._thread);
        return *this;
    }
    ~sp_threadsafe() {
        if (_obj) {
            if (_thread == Thread::current()) {
                _obj->release();
            } else {
                Object* obj = _obj;
                _thread->post([=]() {
                    obj->release();
                });
            }
        }
    }

    T& operator* () { return *(T*)_obj; }
    T* operator->() const { return (T*)_obj; }
    T* operator& () const { return (T*)_obj; }
    operator T*() const { return (T*)_obj; }
    
    T* _obj;
    class Thread* _thread;

};
*/



