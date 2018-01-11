//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"


Window* mainWindow;



float dp(float dp) {
	return dp*mainWindow->_scale;
}
float idp(float pix) {
	return pix/mainWindow->_scale;
}


#ifndef ANDROID
void oakLog(char const* fmt, ...) {
	char ach[2048];
	va_list args;
	va_start(args, fmt);
	vsnprintf(ach, sizeof(ach), fmt, args);
	printf("%s\n", ach);
}
#endif



