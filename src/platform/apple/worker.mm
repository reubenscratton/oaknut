//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>

void Worker::start(const variant& config) {
    assert(!_queue); // already started!
    retain();
    _queue = dispatch_queue_create("OakWorker", DISPATCH_QUEUE_SERIAL);
    auto config_copy = config;
    dispatch_async(_queue, ^{
        _impl->start_(config_copy);
        dispatch_async(dispatch_get_main_queue(), ^() {
            release();
        });
    });
    _started = true;
}

void Worker::dispatchProcessResult(const variant& data_out) {
    // not used
}

void Worker::process(const variant& data_in, std::function<void(const variant&)> callback) {
    if (!_started) {
        app->log("Warning! process() called on stopped worker");
        return;
    }
    assert(_queue); // not started!
    auto data_copy = data_in;
    retain();
    dispatch_async(_queue, ^{
        variant data_out = _impl->process_(data_copy);
        dispatch_async(dispatch_get_main_queue(), ^() {
            callback(data_out);
            release();
        });
    });
}

void Worker::dispatchStopped() {
    // not used
}

void Worker::stop(std::function<void()> onStop) {
    _started = false;
    assert(_queue); // not started!
    retain();
    dispatch_async(_queue, ^{
        _impl->stop_();
        dispatch_async(dispatch_get_main_queue(), ^() {
            _queue = NULL; // allegedly releases the queue too
            onStop();
            release();
        });
    });

}


#endif
