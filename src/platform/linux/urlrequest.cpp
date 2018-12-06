//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include <oaknut.h>

class URLRequestLinux : public URLRequest {
public:
    URLRequestLinux(const string& url, const string& method, const bytearray& body, int flags)
    : URLRequest(url, method, body, flags) {
    }
    
    void run() override {

        CURL *curl;
        CURLcode res;
        
        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, _url.data());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
            curl_easy_cleanup(curl);
        }
    
    }
    
    void cancel() override {
        app.log("todo! URLRequest::cancel");
    }
};


URLRequest* URLRequest::create(const string& url, const string& method, const bytearray& body, int flags) {
    return new URLRequestLinux(url, method, body, flags);
}

#endif
