//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB && OAKNUT_WANT_AUDIOINPUT

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
