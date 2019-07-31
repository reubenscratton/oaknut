//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_LINUX

#include <oaknut.h>

static vector<PosixTaskQueue*> s_queues;
static int s_index;

static struct StaticInit {
    StaticInit() {
        unsigned int numCores = std::thread::hardware_concurrency();
        if (!numCores) {
            numCores = 4;
        }
        for (int i=0 ; i<numCores ; i++) {
            s_queues.push_back(new PosixTaskQueue(string::format("urlrequest-%d", i)));
        }
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }
} s_init;

class URLRequestLinux : public URLRequest {
public:
    URLRequestLinux(const string& url, const string& method, const bytearray& body, int flags)
    : URLRequest(url, method, body, flags) {
    }

    map<string,string> _responseHeaders; // todo: this is really generic
    PosixTaskQueue* _queue;
    int _task_id;

    // curl callbacks
    static size_t header_cb(char *buffer, size_t size, size_t nitems, URLRequestLinux* req) {
        uint r = size * nitems;
        string hdr(buffer, r);
        int i = hdr.find(':');
        if (i>0) {
            string name = hdr.substr(0, i).lowercase();
            string value = hdr.substr(i+1);
            value.trim();
            req->_responseHeaders[name] = value;
        }
        return r;
    } 
    static uint write_cb(char *in, uint size, uint nmemb, URLRequestLinux* req) {
        uint r = size * nmemb;
        req->_responseData.append((uint8_t*)in, r);
        return r;
    }

    void run() override { // todo: a better name would be 'start()'.

        // Choose a queue (TODO: implement priorities)
        _queue = s_queues[s_index];
        s_index = (s_index+1) % s_queues.size();
        _task_id = _queue->enqueueTask([=]() {
            runInBackground();
        });
    }

    void runInBackground() {
        CURL *curl;
        CURLcode res;
        
        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, _url.data());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, this); 
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                app->log("curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
            }
            curl_easy_cleanup(curl);

            if (_cancelled || res != CURLE_OK) { // cancelled, just abort.
                dispatchResult(0, {});
            } else {
                long status=0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
                dispatchResult(status, _responseHeaders);
            }

        }
    
    }
    
    void cancel() override {
        _cancelled = true;
        if(_queue) {
            _queue->cancelTask(_task_id);
            _queue=nullptr;
        }
    }
};


URLRequest* URLRequest::create(const string& url, const string& method, const bytearray& body, int flags) {
    return new URLRequestLinux(url, method, body, flags);
}

#endif
