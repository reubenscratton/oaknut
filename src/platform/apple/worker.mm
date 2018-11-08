//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>

class WorkerApple : public Worker {
public:
    
    void start(const variant& config) override {
        _queue = dispatch_queue_create("OakWorker", DISPATCH_QUEUE_SERIAL);
        dispatch_async(_queue, ^{
            start_(config);
        });
    }
    void process(const bytearray& data_in, std::function<void(const bytearray&)> callback) override {
        dispatch_async(_queue, ^{
            bytearray data_out = process_(data_in);
            dispatch_async(dispatch_get_main_queue(), ^() {
                callback(data_out);
            });
        });
    }
    void stop(std::function<void()> onStop) override {
        dispatch_async(_queue, ^{
            stop_();
            dispatch_async(dispatch_get_main_queue(), ^() {
                _queue = NULL; // allegedly releases the queue too
                onStop();
            });
        });

    }

protected:
    virtual void start_(const variant& config) { }
    virtual const bytearray process_(const bytearray& data_in) = 0;
    virtual void stop_() { }

    dispatch_queue_t _queue;
};

class Mp3Encoder : public WorkerApple {
public:
    void start_(const variant& config) override {
        
    }
    const bytearray process_(const bytearray& data_in) override {
        return bytearray();
    }
    
    void stop_() override {
        
    }

};

Worker* Worker::create(const string& name) {
    if (name == "mp3encoder") {
        return new Mp3Encoder();
    }
    assert(0);
    return NULL;
}


#endif
