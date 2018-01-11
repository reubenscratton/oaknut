//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class UserDefaults {
public:

	static int getInteger(const char* key, const int defaultValue);
	static void setInteger(const char* key, int value);

	static string getString(const char* key, const char* defaultValue);
	static void setString(const char* key, const char* value);
	
	static void save();

};
