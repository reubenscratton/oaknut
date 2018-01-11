//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

static KeyValueMap s_userdefaults;
static bool s_init;

static string getUserDefaultsPath() {
    string str = oakGetAppHomeDir();
    str.append("_userdefaults.dat");
    return str;
}

static void ensureLoaded() {
    if (s_init) {
        return;
    }
    s_init = true;
    FileStream stream(getUserDefaultsPath());
    if (stream.openForRead()) {
        s_userdefaults.readSelfFromStream(&stream);
    }
}


int UserDefaults::getInteger(const char *key, const int defaultValue) {
    ensureLoaded();
    if (s_userdefaults.hasValue(key)) {
        return s_userdefaults.getInt(key);
    }
	return defaultValue;
}


void UserDefaults::setInteger(const char* key, const int value) {
    ensureLoaded();
	s_userdefaults.setInt(key, value);
}

string UserDefaults::getString(const char* key, const char* defaultValue) {
    ensureLoaded();
    if (s_userdefaults.hasValue(key)) {
        return s_userdefaults.getString(key);
    }
	return defaultValue;
}

void UserDefaults::setString(const char* key, const char* value) {
    ensureLoaded();
    s_userdefaults.setString(key, value);
}

void UserDefaults::save() {
    FileStream stream(getUserDefaultsPath());
    if (stream.openForWrite()) {
        s_userdefaults.writeSelfToStream(&stream);
        stream.close();
    }
}

