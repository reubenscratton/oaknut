//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if PLATFORM_WEB

#include <oaknut.h>



class AudioInputWeb : public AudioInput {
public:
    
    AudioInputWeb() {
    }
    
    void open(int sampleRate) override {
    }
    void start() override {
    }
    
    void stop() override {
    }
    void close() override {
    }
    
    
};

AudioInput* AudioInput::create() {
    return new AudioInputWeb();
}


#endif
