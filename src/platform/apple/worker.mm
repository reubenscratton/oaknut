//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import <oaknut.h>

void WorkerApple::start(const variant& config) {
    _queue = dispatch_queue_create("OakWorker", DISPATCH_QUEUE_SERIAL);
    dispatch_async(_queue, ^{
        start_(config);
    });
}
void WorkerApple::process(const variant& data_in, std::function<void(const variant&)> callback) {
    auto data_copy = data_in;
    dispatch_async(_queue, ^{
        variant data_out = process_(data_copy);
        dispatch_async(dispatch_get_main_queue(), ^() {
            callback(data_out);
        });
    });
}
void WorkerApple::stop(std::function<void()> onStop) {
    dispatch_async(_queue, ^{
        stop_();
        dispatch_async(dispatch_get_main_queue(), ^() {
            _queue = NULL; // allegedly releases the queue too
            onStop();
        });
    });

}


#endif
