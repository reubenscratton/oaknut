//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

/**
 UserDefaults is implemented by a single LocalStore containing a single VariantMap
 */

static LocalStore* s_localstore;
static VariantMap* s_userdefaults;


static void ensureLoaded() {
    if (!s_localstore) {
        s_localstore = LocalStore::create("_userdefaults", "unused");
    }
    if (s_localstore->moveFirst()) {
        s_userdefaults = s_localstore->readCurrent();
    }
    if (!s_userdefaults) {
        s_userdefaults = new VariantMap();
        s_userdefaults->setInt("unused", 0); // set a primary key value although it's not used
    }
/*    FileStream stream(getUserDefaultsPath());
    if (stream.openForRead()) {
        s_userdefaults.readSelfFromStream(&stream);
    }*/
}


int UserDefaults::getInteger(const char *key, const int defaultValue) {
    ensureLoaded();
    if (s_userdefaults->hasValue(key)) {
        return s_userdefaults->getInt(key);
    }
	return defaultValue;
}


void UserDefaults::setInteger(const char* key, const int value) {
    ensureLoaded();
	s_userdefaults->setInt(key, value);
}

string UserDefaults::getString(const char* key, const char* defaultValue) {
    ensureLoaded();
    if (s_userdefaults->hasValue(key)) {
        return s_userdefaults->getString(key);
    }
	return defaultValue;
}

void UserDefaults::setString(const char* key, const char* value) {
    ensureLoaded();
    s_userdefaults->setString(key, value);
}

void UserDefaults::save() {
    //s_localstore->save();
    /*FileStream stream(getUserDefaultsPath());
    if (stream.openForWrite()) {
        s_userdefaults->writeSelfToStream(&stream);
        stream.close();
    }*/
}

