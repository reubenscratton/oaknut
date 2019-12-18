//
// Copyright © 2019 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

// TODO: This should be in thread-local storage, not global
static std::list<Object*> s_autodeletePool;

void Object::flushAutodeletePool() {
    if (s_autodeletePool.size() > 0) {
        for (auto it=s_autodeletePool.begin() ; it!=s_autodeletePool.end() ; it++) {
            delete *it;
        }
        s_autodeletePool.clear();
    }
}

Object::Object() : _refs(0) {
    // TODO: DEBUG ONLY: track thread ID
}
Object::~Object() {
}
void Object::retain() {
    // TODO: DEBUG ONLY: assert thread ownership
    _refs++;
}
void Object::release() {
    // TODO: DEBUG ONLY: assert thread ownership
	if (--_refs == 0) {
        s_autodeletePool.push_back(this);
		//delete this;
	}
    assert(_refs >= 0);
}
void* Object::operator new(size_t sz) {
	void* p = malloc(sz);
	memset(p, 0, sz);
	return p;
}

#ifdef DEBUG
string Object::debugDescription() {
    char ach[256];
    sprintf(ach, "%lX", (long)this);
    return string(ach);
}
#endif


map<string, Object* (*)()>* Object::s_classRegister;

/*

template<>
Object* Object::createByName(const string& className) {
    return s_classRegister->find(className)->second();
}
*/

