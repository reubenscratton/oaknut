//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#import <oaknut.h>

/**
 Not Yet Implemented cos not really required for any current work.
 
 First step will be to refactor the open() API so it takes a URL or a bytearray. The bytearray
 would convert to a Blob to an Object URL.
 
 Then add a HTMLVideoElement and connect it to the offscreen Canvas and poll the Canvas
 at the framerate of the video stream.
 */

class VideoPlayerWeb : public VideoPlayer {
public:

    void open(const string& assetPath) override {
        log_warn("NYI!");
    }
    
    void play() override {
        log_warn("NYI!");
    }
    
    void setCurrent(int current) override {
    }

    
    void pause() override {
    }
    
    void close() override {
    }

};

VideoPlayer* VideoPlayer::create() {
    return new VideoPlayerWeb();
}


#endif
