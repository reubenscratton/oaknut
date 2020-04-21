//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

//static MruCache<string> s_urldataCache(4*1024*1024);

static string pathForHash(const sha1_t& hash) {
    string path = string::format("//cache/url/%02X/%02X/", hash.bytes[0], hash.bytes[1]);
    path += string::hex(hash.bytes+2, sizeof(hash.bytes)-2);
    File::resolve(path);
    return path;
}

class URLCache : public Object {
public:
    struct index_entry {
       TIMESTAMP downloadTime;
       TIMESTAMP expiryTime;
       string etag;
       sp<URLResponse> response;
     };

    URLCache() {
        Task::enqueue({
            {Task::IO, [=](variant&) -> variant {
                return File::load_sync("//cache/url/index.dat");
            }},
            {Task::Background, [=](variant& v) -> variant {
                if (v.isByteArray()) {
                    bytestream strm(v.bytearrayRef());
                    while (strm.hasMoreToRead()) {
                        pair<sha1_t, index_entry> e;
                        strm.readBytes(sizeof(e.first), &e.first);
                        strm.readBytes(sizeof(e.second.downloadTime), &e.second.downloadTime);
                        strm.readBytes(sizeof(e.second.expiryTime), &e.second.expiryTime);
                        strm.read(e.second.etag);
                        auto p = _map.emplace(e);
                        const pair<sha1_t, index_entry>& pp = *p.first;
                        _list.push_back(&pp);
                    }
                    v = true;
                }
                _hasLoaded = true;
                return v;
            }}
        });
    }

 

    bool containsItem(const sha1_t& hash, index_entry* item) {
        if (!_hasLoaded) {
            return false;
        }
        auto it = _map.find(hash);
        if (it == _map.end()) {
            return false;
        }
        *item = it->second;
        return true;
    }
    
    
    void storeRawResponse(const sha1_t& hash, URLResponse* response) {
        response->retain();
        Task::enqueue({
            {Task::IO, [=](variant&) -> variant {
                string path = pathForHash(hash);
                bytestream strm;
                strm.write(response->httpStatus);
                strm.write(response->headers);
                strm.write(response->data);
                File::save_sync(path, strm.getWrittenBytes());
                return variant();
            }},
            {Task::MainThread, [=](variant&) -> variant {
                auto it = _map.find(hash);
                if (it == _map.end()) {
                    index_entry new_entry;
                    it = _map.emplace(hash, new_entry).first;
                }
                it->second.downloadTime = app->currentMillis();
                response->release();

                return variant();
            }}
        });
    }
    void setProcessedResponse(const sha1_t& hash, URLResponse* processedResponse) {
        auto it = _map.find(hash);
        if (it != _map.end()) {
            it->second.response = processedResponse;
        }
    }

//    void save();


protected:

    bool _hasLoaded;
    map<sha1_t, index_entry> _map;
    list<const pair<sha1_t, index_entry>*> _list;
};

static URLCache* s_cache;


URLRequest::URLRequest(const string& url, const string& method, const bytearray& body,
        Object* owner, int flags) : _url(url), _owner(owner) {
    if (!s_cache) {
        s_cache = new URLCache();
    }
	_method = method;
    _body = body;
    _flags = flags;
    _cachePolicy = CachePolicy::Default;
    _status = Status::Queued;
    os_sem_init(&_sem, 0);
}

URLRequest* URLRequest::createAndStart(const string& url, const string& method,
        const bytearray& body, Object* owner, int flags) {
    
    // TODO: lookup existing request by url?
    
    auto req = new URLRequest(url, method, body, owner, flags);
    req->retain();
    Task::postToMainThread([=]() {
        req->start();
        req->release();
    });
    return req;
}
URLRequest* URLRequest::get(const string& url, Object* owner, int flags/*=0*/) {
    return createAndStart(url, "GET", bytearray(), owner, flags);
}
URLRequest* URLRequest::post(const string& url, const bytearray& body, Object* owner) {
    return createAndStart(url, "POST", body, owner, 0);
}
URLRequest* URLRequest::patch(const string& url, const bytearray& body, Object* owner) {
    return createAndStart(url, "PATCH", body, owner, 0);
}

URLRequest::~URLRequest() {
    os_sem_delete(_sem);
}

void URLRequest::setHeader(const string &headerName, const string &headerValue) {
    _headers[headerName] = headerValue;
}
void URLRequest::handle(std::function<void(const URLResponse*,bool)> handler) {
    _handler = handler;
}

void URLRequest::start() {
    //app->log("start: %s", _url.c_str());
    
    const sha1_t sha = sha1(_url);

    // See if response is in RAM cache, if so we can avoid a cache load.
    bool cacheLoadWanted = (_cachePolicy != CachePolicy::RemoteOnly);
    URLCache::index_entry item;
    if (cacheLoadWanted) {
        if (s_cache->containsItem(sha, &item)) {
            if (item.response) {
                dispatchResponse(item.response, true);
                cacheLoadWanted = false;
            }
        } else {
            cacheLoadWanted = false;
        }
    }
    
    // Enqueue the disk cache load
    if (cacheLoadWanted) {
        retain();
        _cacheTask = Task::enqueue({
            {Task::IO, [=](variant&)->variant {
                string path = pathForHash(sha);
                variant data = File::load_sync(path);
                if (!data.isByteArray()) {
                    // TODO: if we opened it but couldn't read it, maybe delete the file?
                    return nullptr;
                }
                bytestream strm(data.bytearrayRef());
                URLResponse* response = new URLResponse();
                strm.read(response->httpStatus);
                strm.read(response->headers);
                strm.read(response->data);
                return response;
            }},
            {Task::Background, [=](variant& result)->variant {
                // If the cache load failed OR the remote load already
                // completed, OR the whole request was cancelled then just return empty
                if (!result.isPtr() || _remoteLoadComplete || _status==Cancelled) {
                    return variant();
                }
                URLResponse* response = result.ptr<URLResponse>();
                processRawResponse(response);
                return response;
            }},
            {Task::MainThread, [=](variant& result)->variant {
                if (!(!result.isPtr() || _remoteLoadComplete || _status==Cancelled)) {
                    URLResponse* processedResponse = result.ptr<URLResponse>();
                    s_cache->setProcessedResponse(sha, processedResponse);
                    dispatchResponse(processedResponse, true);
                }
                release();
                return variant();
            }}
        });
    }
    
    // Queue remote load
    if (_cachePolicy != CachePolicy::CacheOnly) {
        retain();
        _remoteTask = Task::enqueue({
            {Task::IO, [=](variant&) -> variant {
                URLResponse* remoteResponse = new URLResponse();
                error err = ioLoadRemote(remoteResponse);
                if (err) {
                    delete remoteResponse;
                    return err;
                }
                return remoteResponse;
            }},
            {Task::Background, [=](variant& result) -> variant {
                if (!result.isPtr() || _status==Cancelled) {
                    return variant();
                }
                _remoteLoadComplete = true;
                URLResponse* response = result.ptr<URLResponse>();
                s_cache->storeRawResponse(sha, response);
                processRawResponse(response);
                return response;
            }},
            {Task::MainThread, [=](variant& result) -> variant {
                if (!result.isPtr() || _status==Cancelled) {
                    release();
                    return variant();
                }
                sp<URLResponse> processedResponse = result.ptr<URLResponse>();
                s_cache->setProcessedResponse(sha, processedResponse);
                dispatchResponse(processedResponse, false);
                release();
                return variant();
            }},
        });
    }
}



void URLRequest::cancel() {
    _status = Status::Cancelled;
    if (_cacheTask) {
        _cacheTask->cancel();
        _cacheTask = nullptr;
    }
    if (_remoteTask) {
        os_sem_signal(_sem);
        _remoteTask->cancel();
        _remoteTask = nullptr;
    }
    if (_owner) {
        _owner = nullptr;
    }
}

bool URLRequest::isCancelled() const {
    return _status == Status::Cancelled;
}

void URLRequest::dispatchResponse(const URLResponse* response, bool isFromCache) {
    if (_handler && _status != Cancelled) {
        _handler(response, isFromCache);
    }
    if (_owner) {
        _owner = nullptr;
    }
}

/**
 processResponse is called on the background to perform decoding of the raw byte response into
 a usable form, e.g. decoding a bitmap or parsing a JSON object, etc.
 */
void URLRequest::processRawResponse(URLResponse* response) {
    bool didError = (response->httpStatus>=400) || _status==Status::Error;
    
    // Give app code a chance to process the response in the background.
    if (customDecoder) {
        if (customDecoder(response)) {
            return;
        }
    }

    // Default decoding
    if (!didError) {
        string contentType;
        const auto& contentTypeIt = response->headers.find("content-type");
        if (contentTypeIt != response->headers.end()) {
            contentType = contentTypeIt->second;
        }
        
        // Bitmaps
        if (contentType.hasPrefix("image/") && response->data.length()) {
            response->decoded.bitmap = Bitmap::createFromData(response->data);
        }

        // JSON
        else if (contentType.contains("json")) {
            string str = response->data.toString();
            response->decoded.json = variant::parse(str, PARSEFLAG_JSON);
        }
        
        // Other text form
        else if (contentType.hasPrefix("text/")) {
            // TODO: Handle charset encodings here. At present we just presume UTF-8.
            response->decoded.text = response->data.toString();
        }
    }

}


    


