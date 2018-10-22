//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

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
}
Object::~Object() {
}
void Object::retain() {
    _refs++; // TODO: Should be atomic to cover rare cases where retain/release from worker thread
}
void Object::release() {
	if (--_refs == 0) {
        s_autodeletePool.push_back(this); // TODO: protect with mutex
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


map<string, Object* (*)()>* oak::s_classRegister;

Object* Object::createByName(const string& className) {
    return s_classRegister->find(className)->second();
}
