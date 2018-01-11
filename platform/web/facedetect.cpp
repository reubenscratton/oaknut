//
//  Copyright © 2017 Post-Quantum. All rights reserved.
//

#include "../../src/oaknut.h"

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
