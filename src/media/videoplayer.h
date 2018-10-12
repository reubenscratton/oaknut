//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class VideoPlayer : public Object {
public:
    
    enum Status {
        Unready,
        Ready,
        Playing,
        Paused,
        Finished
    };
    
    // API
    static VideoPlayer* create();
    virtual void open(const string& assetPath)=0;
    virtual bool getStatus() const { return _status; }
    virtual void setCurrent(int current) = 0;
    virtual void play()=0;
    virtual void pause()=0;
    virtual void close()=0;
    
    virtual void playToggle();

    std::function<void()> onReady;
    std::function<void(Bitmap*)> onNewFrameReady;
    
protected:
    VideoPlayer();
    int _current;
    Status _status;
    void setStatus(Status newStatus);
};

