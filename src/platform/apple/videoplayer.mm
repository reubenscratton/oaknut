//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>
#import <AVFoundation/AVFoundation.h>

@interface AVPlayerHelper : NSObject
- (id)initWithPlayer:(class VideoPlayerApple*)player;
- (void)close;
@end




class VideoPlayerApple : public VideoPlayer {
public:
    AVPlayerHelper* _helper;

    void open(const string& assetPath) override {
        assert(_status == Unready);
        NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:assetPath.data()]];
        _player = [[AVPlayer alloc] initWithURL:url];
        _helper = [[AVPlayerHelper alloc] initWithPlayer:this];
        NSDictionary* settings = @{ (id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA) };
        _output = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:settings];
        [_player.currentItem addOutput:_output];
    }
    
    CFTimeInterval _timeStartedPlaying;
    
    void play() override {
        if (_status == Unready || _status == Playing) {
            return;
        }
        _timeStartedPlaying = CACurrentMediaTime();
        setStatus(Playing);
        _timer = Timer::start([=]() {
            CMTime outputItemTime = [_output itemTimeForHostTime: CACurrentMediaTime()];
            if ([_output hasNewPixelBufferForItemTime:outputItemTime]) {
                CMTime presentationItemTime = kCMTimeZero;
                CVPixelBufferRef pixBuff = [_output copyPixelBufferForItemTime:outputItemTime itemTimeForDisplay:&presentationItemTime];
                
                ObjPtr<Bitmap> bitmap = new Bitmap(pixBuff, false);
                onNewFrameReady(bitmap);
                
                CVBufferRelease( pixBuff );
            }
        }, 15, true);
        
        [_player play];
    }
    
    void onStatusChanged() {
        if (_player.status == AVPlayerStatusReadyToPlay) {
            setStatus(Ready);
        }
    }
    
    void onFinished() {
        pause();
        setStatus(Finished);
    }
    
    void setCurrent(int current) override {
        [_player seekToTime:CMTimeMake(current, 1000)];
    }

    
    void pause() override {
        if (_status != Playing) {
            return;
        }
        _timer->stop();
        _timer = NULL;
        [_player pause];
        setStatus(Paused);
    }
    
    void close() override {
        pause();
        [_helper close];
        _helper = NULL;
        _player = NULL;
        _output = NULL;
        setStatus(Unready);
    }

    AVPlayer* _player;
    AVPlayerItemVideoOutput* _output;
    Timer* _timer;
};

VideoPlayer* VideoPlayer::create() {
    return new VideoPlayerApple();
}


@implementation AVPlayerHelper {
    VideoPlayerApple* _player;
}

- (id)initWithPlayer:(VideoPlayerApple*)player {
    if (self = [super init]) {
        _player = player;
        [player->_player addObserver:self forKeyPath:@"status" options:(NSKeyValueObservingOptionNew | NSKeyValueObservingOptionInitial) context:player];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(playerDidFinishPlaying:)
                                                     name: AVPlayerItemDidPlayToEndTimeNotification
                                                   object: player->_player.currentItem];

    }
    return self;
}

- (void)playerDidFinishPlaying:(NSNotification*)notification {
    _player->onFinished();
}


- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary<NSKeyValueChangeKey, id> *)change
                       context:(void *)context {
    if ([@"status" isEqualToString:keyPath]) {
        VideoPlayerApple* player = (VideoPlayerApple*)context;
        player->onStatusChanged();
    }
}

- (void)close {
    [_player->_player removeObserver:self forKeyPath:@"status"];
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                 name: AVPlayerItemDidPlayToEndTimeNotification
                                               object: _player->_player.currentItem];
    _player = NULL;
}

@end
#endif
