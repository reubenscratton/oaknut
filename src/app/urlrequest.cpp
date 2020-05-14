//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


static MruCache<sha1_t, URLResponse*> s_urlRamCache(4*1024*1024);

static string pathForHash(const sha1_t& hash) {
    string path = string::format("//cache/url/%02X/%02X/", hash.bytes[0], hash.bytes[1]);
    path += string::hex(hash.bytes+2, sizeof(hash.bytes)-2);
    File::resolve(path);
    return path;
}



URLRequest::URLRequest(const string& url, const string& method, const bytearray& body,
        Object* owner, int flags) : _url(url), _owner(owner) {
	_method = method;
    _body = body;
    _flags = flags;
    _cachePolicy = CachePolicy::Default;
    _status = Status::Queued;
}

URLRequest* URLRequest::createAndStart(const string& url, const string& method,
        const bytearray& body, Object* owner, int flags) {
    
    // TODO: lookup existing request by url?
    log("createAndStart: %s", url.c_str());
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
}

void URLRequest::setHeader(const string &headerName, const string &headerValue) {
    _headers[headerName] = headerValue;
}
void URLRequest::handle(std::function<void(const URLResponse*,bool)> handler) {
    _handler = handler;
}

void URLRequest::start() {
    log("start: %s", _url.c_str());
    
    const sha1_t sha = sha1(_url);

    // See if response is in RAM cache, if so we can avoid a cache load.
    bool cacheLoadWanted = (_cachePolicy != CachePolicy::RemoteOnly);
    URLResponse* cacheResponse = nullptr;
    if (cacheLoadWanted) {
        if (s_urlRamCache.get(sha, &cacheResponse)) {
            log("RAMcache hit: %s", _url.c_str());
            _cachedResponse = cacheResponse;
            dispatchResponse(cacheResponse, true);
            cacheLoadWanted = false;
        }
    }
     

    // Enqueue the disk cache load
    if (cacheLoadWanted) {
        retain();
        _cacheTask = Task::enqueue({
            {Task::IO, [=](variant&)->variant {
                string path = pathForHash(sha);
                // TODO: don't load whole thing into RAM here, read in two parts so we can
                // spawn remote load before data has been read.
                variant data = File::load_sync(path);
                if (data.isByteArray()) {
                    log("DiskCache hit: %s", _url.c_str());
                    bytestream strm(data.bytearrayRef());
                    _cachedResponse = new URLResponse();
                    strm.read(_cachedResponse->httpStatus);
                    strm.read(_cachedResponse->headers);
                    strm.read(_cachedResponse->downloadTime);
                    strm.read(_cachedResponse->expiryTime);
                    strm.read(_cachedResponse->data);
                }
                if (_cachePolicy != CachePolicy::CacheOnly) {
                    startRemoteLoad(sha);
                }
                return true;
            }},
            {Task::Background, [=](variant& result)->variant {
                if (_cachedResponse) {
                    processRawResponse(_cachedResponse);
                }
                return true;
            }},
            {Task::MainThread, [=](variant& result)->variant {
                if (_cachedResponse && !(_remoteLoadComplete || _status==Cancelled)) {
                    log("Disk->RAM cache: %s", _url.c_str());
                    s_urlRamCache.put(sha, _cachedResponse._obj, 1024*16);
                    dispatchResponse(_cachedResponse, true);
                }
                release();
                return variant();
            }}
        });
    }
    
    // If we don't care about cache, kick off the remote load immediately
    if (!cacheLoadWanted) {
        startRemoteLoad(sha);
    }
}

void URLRequest::startRemoteLoad(const sha1_t& sha) {
    assert(!_remoteTask);
    retain();
    _remoteTask = Task::enqueue({
        {Task::IO, [=](variant&) -> variant {
            
            if (_status==Cancelled) {
                return variant::empty();
            }
            
            // Instantiate and fetch the remote data
            _remoteResponse = new URLResponse();
            log("ioLoadRemote: %s", _url.c_str());
            error err = ioLoadRemote();
            if (err) {
                return err;
            }
            _remoteResponse->downloadTime = app->currentMillis();
            
            // Set an error return value if there was an HTTP error
            if (_remoteResponse->httpStatus >= 400) {
                return error(_remoteResponse->httpStatus);
            }
            
            // All good so far
            return true;
        }},
        {Task::Background, [=](variant& result) -> variant {
            if (result.isEmpty() || result.isError() || _status==Cancelled) {
                return result;
            }
            
            // If the cached version was ok, nothing else to do in background
            if (_cachedResponse && _remoteResponse->httpStatus == 304) {
                log("304: %s", _url.c_str());
                // TODO: extend the expiry date of the cache entry
                return true;
            }

            _remoteLoadComplete = true;

            // Spawn a separate IO task to store the response on disk
            retain();
            Task::enqueue({
                {Task::IO, [=](variant&) -> variant {
                    log("DiskCache put: %s", _url.c_str());
                    string path = pathForHash(sha);
                    bytestream strm;
                    strm.write(_remoteResponse->httpStatus);
                    strm.write(_remoteResponse->headers);
                    strm.write(_remoteResponse->downloadTime);
                    strm.write(_remoteResponse->expiryTime);
                    strm.write(_remoteResponse->data);
                    File::save_sync(path, strm.getWrittenBytes());
                    release();
                    return variant();
                }},
            });

            processRawResponse(_remoteResponse);
            return true;
        }},
        {Task::MainThread, [=](variant& result) -> variant {
            if (result.isEmpty() || result.isError() || _status==Cancelled) {
                release();
                return result;
            }
            if (_cachedResponse && _remoteResponse->httpStatus == 304) {
            } else {
                log("Remote -> RAM: %s", _url.c_str());
                s_urlRamCache.put(sha, _remoteResponse._obj, 16*1024);
                dispatchResponse(_remoteResponse, false);
            }
            release();
            return variant();
        }},
    });

}


void URLRequest::cancel() {
    _status = Status::Cancelled;
    if (_cacheTask) {
        log("Cancel cache: %s", _url.c_str());
        _cacheTask->cancel();
        _cacheTask = nullptr;
    }
    if (_remoteTask) {
        log("Cancel remote: %s", _url.c_str());
        _sem.signal();
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
    //bool didError = (response->httpStatus>=400) || _status==Status::Error;
    
    // Give app code a chance to process the response in the background.
    if (customDecoder) {
        if (customDecoder(response)) {
            return;
        }
    }

    // Default decoding
    //if (!didError) {
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
    //}

}


    


