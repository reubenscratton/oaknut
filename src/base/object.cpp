//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


Object::Object() : _refs(0)
{
}
Object::~Object() {
}
void Object::retain() {
#if DEBUG && !WANT_ATOMIC_REFCOUNTS
    if (!_refs) {
        _owningThreadId = std::this_thread::get_id();
    } else {
        assert(_owningThreadId == std::this_thread::get_id());
    }
#endif
    _refs++;
}
void Object::release() {
#if DEBUG && !WANT_ATOMIC_REFCOUNTS
    assert(_owningThreadId == std::this_thread::get_id());
#endif
    assert(_refs > 0);
	if (--_refs == 0) {
        //Task::addObjectToCurrentThreadDeletePool(this);
		delete this;
	}
}
void* Object::operator new(size_t sz) {
	void* p = malloc(sz);
	memset(p, 0, sz);
	return p;
}

uint32_t Object::getRamCost() const {
    assert(0); // you should override this with an appropriate implementation!
}

#ifdef DEBUG
string Object::debugDescription() {
    char ach[256];
    sprintf(ach, "%lX", (long)this);
    return string(ach);
}
#endif


map<string, Object* (*)()>* Object::s_classRegister;


