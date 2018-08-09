//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import "oaknut.h"


static void dispatchResult(URLRequest* req, URLData* urldata) {
    dispatch_async(dispatch_get_main_queue(), ^{
        if (urldata) {
            req->dispatchOnLoad(urldata);
        }
        req->release();
    });
}

void URLRequest::nativeStart() {
    retain(); // ensure request is alive until it completes, can't count on anything else to keep us alive
    NSString* urlstr = [NSString stringWithCString:_url.data() encoding:[NSString defaultCStringEncoding]];
    NSURLRequest* req = [[NSURLRequest alloc] initWithURL:[NSURL URLWithString:urlstr] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:15];
    NSURLSessionDataTask* dataTask = [[NSURLSession sharedSession] dataTaskWithRequest:req completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        if (data.length==0 || error.code == -999) { // cancelled via stop_(), just abort. 'this' might well be deleted.
            dispatchResult(this, NULL);
        } else {
            NSHTTPURLResponse* httpResponse = (NSHTTPURLResponse*)response;
            NSString* contentType = httpResponse.allHeaderFields[@"Content-Type"];
            if ([@"image/jpeg" isEqualToString:contentType]) {
                Bitmap::createFromData(data.bytes, (int)data.length, [&](Bitmap* bitmap) {
                    dispatchResult(this, new URLData(bitmap));
                });
                return;
            } else if ([@"image/png" isEqualToString:contentType]) {
                Bitmap::createFromData(data.bytes, (int)data.length, [&](Bitmap* bitmap) {
                    dispatchResult(this, new URLData(bitmap));
                });
                return;
            }
            // Get a contiguous copy of the data
            ByteBuffer* emdata = new ByteBuffer(data.length);
            [data enumerateByteRangesUsingBlock:^(const void * _Nonnull bytes, NSRange byteRange, BOOL * _Nonnull stop) {
                memcpy(emdata->data+byteRange.location, bytes, byteRange.length);
            }];
            dispatchResult(this, new URLData(emdata));
        }
    }];
    _osobj = (__bridge_retained void*)dataTask;
    [dataTask resume];
}

void URLRequest::nativeStop() {
    NSURLSessionDataTask* dataTask = (__bridge_transfer NSURLSessionDataTask*)_osobj;
    if (dataTask.state == NSURLSessionTaskStateRunning) {
        [dataTask cancel];
    }
}

#endif
