//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_APPLE

#import "oaknut.h"

static NSURLSession* s_session;


extern string nsstr(NSString* s);
extern error nserr(NSError* e);



error URLRequest::ioLoadRemote(URLResponse* response) {

    // One-off init of session object
    if (!s_session) {
        NSURLSessionConfiguration* config = [NSURLSessionConfiguration defaultSessionConfiguration];
        s_session = [NSURLSession sessionWithConfiguration:config];
    }
        
    // Build the native request
    NSString* urlstr = [NSString stringWithCString:_url.c_str() encoding:[NSString defaultCStringEncoding]];
    NSMutableURLRequest* req = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:urlstr]];
    req.HTTPMethod = [NSString stringWithUTF8String:_method.c_str()];
    req.timeoutInterval = 15;
    if (_body.length() > 0) {
        req.HTTPBody = [NSData dataWithBytes:_body.data() length:_body.size()];
    }
    for (auto& header : _headers) {
        NSString* headerName = [NSString stringWithUTF8String:header.first.c_str()];
        NSString* headerValue = [NSString stringWithUTF8String:header.second.c_str()];
        [req setValue:headerValue forHTTPHeaderField:headerName];
    }
    req.cachePolicy = NSURLRequestReloadIgnoringCacheData;
    
    // Execute the native request and block this IO thread on our semaphore until it's signalled
    __block error err = error::none();
    NSURLSessionDataTask* dataTask = [s_session dataTaskWithRequest:req completionHandler:^(NSData* data, NSURLResponse* res, NSError* error) {
        if (_status!=Cancelled) {
            if (error) {
                _status = Status::Error;
                err = nserr(error);
            } else {
                NSHTTPURLResponse* httpResponse = (NSHTTPURLResponse*)res;
                response->httpStatus = (int)httpResponse.statusCode;
                
                // Get all response headers into a generic container
                //  Cache-Control: public, max-age=31536000
                response->headers.clear();
                for (NSString* headerName in httpResponse.allHeaderFields.allKeys) {
                    response->headers[nsstr([headerName lowercaseString])] = nsstr(httpResponse.allHeaderFields[headerName]);
                }

                // Read the response data into contiguous memory
                __block bytearray responseData;
                [data enumerateByteRangesUsingBlock:^(const void * _Nonnull bytes, NSRange byteRange, BOOL * _Nonnull stop) {
                    responseData.append((uint8_t*)bytes, (int32_t)byteRange.length);
                }];
                response->data = responseData;
            }
        }
        os_sem_signal(_sem);
    }];
    [dataTask resume];
    
    // Wait for either request to complete or for cancel().
    os_sem_wait(_sem);
    
    // If semaphore signalled by cancel(), the data task has not completed so cancel it.
    if (_status == Status::Cancelled) {
        [dataTask cancel];
    }
    
    return err;
}



#endif
