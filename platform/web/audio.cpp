//
//  Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//

#if OAKNUT_WANT_AUDIOINPUT
#include <oaknut.h>

const void* oakAudioInputOpen(int sampleRate) { // samples are always int16_t, always record in mono.
    return (void*)1;
}

void oakAudioInputStart(const void* osobj, AudioInputDelegate delegate) {
}
void oakAudioInputStop(const void* osobj) {
}
void oakAudioInputClose(const void* osobj) {
}

#endif
