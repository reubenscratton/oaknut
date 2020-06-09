//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


static MruCache<sha1_t, sp<URLResponse>> s_urlRamCache;

static struct init {
    init() {
        auto cacheConfig = app->getStyle("app.cache");
        s_urlRamCache.setSize(cacheConfig->intVal("ram"));
    }
} _init;

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
    log_dbg("start: %s", _url.c_str());
    
    const sha1_t sha = sha1(_url);

    // See if response is in RAM cache, if so we can avoid a cache load.
    bool cacheLoadWanted = (_cachePolicy != CachePolicy::RemoteOnly);
    if (cacheLoadWanted) {
        if (s_urlRamCache.get(sha, _cachedResponse)) {
            log_dbg("RAMcache hit: %s", _url.c_str());
            dispatchResponse(_cachedResponse, true);
            cacheLoadWanted = false;
        }
    }
     

    // Enqueue the disk cache load
    if (cacheLoadWanted) {
        retain();
        _cacheTask = Task::enqueue({
            {Task::IO, [=](variant&)->variant {
                string path = pathForHash(sha);
                bool cacheValid = false;
                // TODO: don't load whole thing into RAM here, read in two parts so we can
                // spawn remote load before data has been read.
                variant data = File::load_sync(path);
                if (data.isByteArray()) {
                    log_dbg("DiskCache hit: %s", _url.c_str());
                    bytestream strm(data.bytearrayRef());
                    _cachedResponse = new URLResponse();
                    strm.read(_cachedResponse->expiryTime);
                    strm.read(_cachedResponse->httpStatus);
                    strm.read(_cachedResponse->headers);
                    strm.read(_cachedResponse->downloadTime);
                    strm.read(_cachedResponse->data);
                    cacheValid = app->currentMillis() <= _cachedResponse->expiryTime;
                }
                if (_cachePolicy == CachePolicy::CacheOnly) {
                    return true;
                }
                if (cacheValid && _cachePolicy != CachePolicy::RemoteOnly) {
                    log_dbg("Cache valid: %s", _url.c_str());
                    return true;
                }
                startRemoteLoad(sha);
                return true;
            }},
            {Task::Background, [=](variant& result)->variant {
                if (_remoteLoadComplete || _status==Cancelled) {
                    return true;
                }
                if (_cachedResponse) {
                    processRawResponse(_cachedResponse);
                }
                return true;
            }},
            {Task::MainThread, [=](variant& result)->variant {
                if (_cachedResponse && !(_remoteLoadComplete || _status==Cancelled)) {
                    if (_cachedResponse->hasBeenDecoded) {
                        _cachedResponse->data.clear();
                    }
                    log_dbg("Disk->RAM cache: %s", _url.c_str());
                    s_urlRamCache.put(sha, _cachedResponse, _cachedResponse->getRamCost());
                    dispatchResponse(_cachedResponse, true);
                    if (_cachePolicy == CacheOnly) {
                        _owner = nullptr;
                    }
                }
                release();
                return variant();
            }}
        }, _owner);
    }
    
    // If we don't care about cache, kick off the remote load immediately
    if (!cacheLoadWanted) {
        if (_cachedResponse) {
            bool cacheValid = app->currentMillis() <= _cachedResponse->expiryTime;
            if (cacheValid) {
                _owner = nullptr;
                return;
            }
        }
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
            log_dbg("ioLoadRemote: %s", _url.c_str());
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
            
            // Calculate expiry time from Expires and/or Cache-Control headers
            _remoteResponse->expiryTime = 0;
            string expires = _remoteResponse->getHeader("Expires");
            if (expires.length()) {
                // See https://tools.ietf.org/html/rfc7231#section-7.1.1.1
                assert(expires.length() == 29); // Only support the preferred format at present
                auto s = expires.c_str();
                // Example: Wed, 21 Oct 2015 07:28:00 GMT
                struct tm t;
                memset(&t, 0, sizeof(t));
                t.tm_mday = (s[5]-'0') * 10 +
                            (s[6]-'0');
                t.tm_mon = string("JanFebMarAprMayJunJulAugSepOctNovDev").find(s+8, 3) / 3;
                t.tm_year = (s[12]-'0') * 1000 +
                            (s[13]-'0') * 100 +
                            (s[14]-'0') * 10 +
                            (s[15]-'0');
                t.tm_hour = (s[17]-'0') * 10 +
                            (s[18]-'0');
                t.tm_min =  (s[20]-'0') * 10 +
                            (s[21]-'0');
                t.tm_sec =  (s[23]-'0') * 10 +
                            (s[24]-'0');
                t.tm_zone = (char*)s+26;
                double maxAge = difftime(mktime(&t), time(nullptr));
                _remoteResponse->expiryTime = app->currentMillis() + maxAge * 1000;
            }
            string cacheControl = _remoteResponse->getHeader("Cache-Control");
            auto directives = cacheControl.split(",");
            for (auto directive : directives) {
                directive.trim();
                if (directive.hadPrefix("max-age=")) {
                    int maxAge = atoi(directive.c_str());
                    _remoteResponse->expiryTime = app->currentMillis() + maxAge * 1000;
                }
                // NB: Policy decision to deliberately ignore all other directives, cos servers
                // for app backends are seldom configured sensibly. Also, in general it's a better
                // UX when apps are able to start from where they left off rather than showing
                // spinners etc.
            }

            _remoteLoadComplete = true;

            // Spawn a separate IO task to update the disk cache
            // NB: We do this even if request cancelled, to avoid wasting the downloaded response.
            retain();
            Task::enqueue({
                {Task::IO, [=](variant&) -> variant {
                    string path = pathForHash(sha);
                    if (_cachedResponse && _remoteResponse->httpStatus == 304) {
                        log_dbg("HTTP 304: %s", _url.c_str());
                        _cachedResponse->expiryTime = _remoteResponse->expiryTime;
                        int fd = ::open(path.c_str(), O_WRONLY, S_IRUSR|S_IWUSR);
                        if (-1 == fd) {
                            return error::fromErrno();
                        }
                        ::lseek(fd, 0, SEEK_SET);
                        ::write(fd, &_remoteResponse->expiryTime, sizeof(_remoteResponse->expiryTime));
                        ::close(fd);
                    } else {
                        log_dbg("DiskCache store: %s", _url.c_str());
                        bytestream strm;
                        strm.write(_remoteResponse->expiryTime);
                        strm.write(_remoteResponse->httpStatus);
                        strm.write(_remoteResponse->headers);
                        strm.write(_remoteResponse->downloadTime);
                        strm.write(_remoteResponse->data);
                        File::save_sync(path, strm.getWrittenBytes());
                        if (_remoteResponse->hasBeenDecoded) {
                            _remoteResponse->data.clear();
                        }
                    }
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
                log_dbg("Remote -> RAM: %s", _url.c_str());
                s_urlRamCache.put(sha, _remoteResponse, _remoteResponse->getRamCost());
                dispatchResponse(_remoteResponse, false);
            }
            _owner = nullptr;
            release();
            return variant();
        }},
    }, _owner);

}


void URLRequest::cancel() {
    _status = Status::Cancelled;
    if (_cacheTask) {
        log_dbg("Cancel cache: %s", _url.c_str());
        _cacheTask->cancel();
        _cacheTask = nullptr;
    }
    if (_remoteTask) {
        log_dbg("Cancel remote: %s", _url.c_str());
        _sem.signal();
        _remoteTask->cancel();
        _remoteTask = nullptr;
    }
    _owner = nullptr;
}

bool URLRequest::isCancelled() const {
    return _status == Status::Cancelled;
}

void URLRequest::dispatchResponse(const URLResponse* response, bool isFromCache) {
    if (_handler && _status != Cancelled) {
        _handler(response, isFromCache);
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
            response->hasBeenDecoded = true;
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
            response->decoded = Bitmap::createFromData(response->data);
            response->hasBeenDecoded = (response->decoded.objectVal<Bitmap>() != nullptr);
        }

        // JSON
        else if (contentType.contains("json")) {
            string str = response->data.toString();
            response->decoded = variant::parse(str, PARSEFLAG_JSON);
            response->hasBeenDecoded = true;
        }
        
        // Other text form
        else if (contentType.hasPrefix("text/")) {
            // TODO: Handle charset encodings here. At present we just presume UTF-8.
            response->decoded = response->data.toString();
            response->hasBeenDecoded = true;
        }
    //}

}


string URLResponse::getHeader(const string& headerName) {
    auto it = headers.find(headerName.lowercase());
    if (it == headers.end()) {
        return string();
    }
    return it->second;
}

uint32_t URLResponse::getRamCost() const {
    uint32_t cost = 0;
    if (!decoded.isEmpty()) {
        cost = decoded.getRamCost();
    } else {
        cost = data.size();
    }
    return cost;
}


