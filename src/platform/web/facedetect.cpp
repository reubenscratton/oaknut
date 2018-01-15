//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>

static int num = 0;

void* oakFaceDetectorCreate() {
    return (void*)1;
}
int oakFaceDetectorDetectFaces(void* osobj, Bitmap* bitmap) {
    num++;
    return (num>=3) ? 1 : 0;
}
void oakFaceDetectorClose(void* osobj) {
}

#endif
