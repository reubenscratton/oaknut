//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

static list<Object*> s_autodeletePool;
#if DEBUG
void* DBGOBJ = 0;
#endif

void Object::flushAutodeletePool() {
    if (s_autodeletePool.size() > 0) {
        for (auto it=s_autodeletePool.begin() ; it!=s_autodeletePool.end() ; it++) {
            delete *it;
        }
        s_autodeletePool.clear();
    }
}

Object::Object() : _refs(0) {
	//app.log("new %s", getClassName(this).data());
}
Object::~Object() {
}
void Object::retain() {
    //assert(_refs<1000);
	_refs++;
}
void Object::release() {
    //assert(_refs<1000);
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
