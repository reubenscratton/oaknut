//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



//static MruCache<string> s_urldataCache(4*1024*1024);
namespace oak {

class URLRequestManager {
public:
    void enqueue(URLRequest* req) {
        req->_it = _queue.insert(_queue.end(), req);
        reschedule();
    }
    void unqueue(URLRequest* req) {
        if (req->_it != _queue.end()) {
            _queue.erase(req->_it);
            req->_it = _queue.end();
        }
        if (req == _active) {
            req->cancel();
            _active = nullptr;
            reschedule();
        }
    }

    void reschedule() {
        App::postToMainThread([=]() {
            drain();
        });
    }
    void drain() {
        if (!_active) {
            _active = _queue.front();
            if (_active) {
                _queue.erase(_active->_it);
                _active->_it = _queue.end();
                _active->load();
            }
        }
    }
    void complete(URLRequest* req) {
        if (req == _active) {
            _active = nullptr;
        }
        drain();
    }
    
//protected:
    list<URLRequest*> _queue;
    URLRequest* _active;
    
    URLCache _cache;
protected:
};



static URLRequestManager s_manager;

};

URLRequest::URLRequest(const string& url, const string& method, const bytearray& body, int flags) : _url(url) {
	_method = method;
    _body = body;
    _flags = flags;
    _cachePolicy = CachePolicy::Default;
    _status = Status::Queued;
}

URLRequest* URLRequest::createAndStart(const string& url, const string& method, const bytearray& body, int flags) {
    
    // TODO: lookup existing request by url?
    
    auto req = create(url, method, body, flags);
    // req->retain(); // paired with a release() in URLRequest::dispatch(). This keeps the request alive until completed.
    s_manager.enqueue(req);
    return req;
}
URLRequest* URLRequest::get(const string& url, int flags/*=0*/) {
    return createAndStart(url, "GET", bytearray(), flags);
}
URLRequest* URLRequest::post(const string& url, const bytearray& body) {
    return createAndStart(url, "POST", body, 0);
}
URLRequest* URLRequest::patch(const string& url, const bytearray& body) {
    return createAndStart(url, "PATCH", body, 0);
}

URLRequest::~URLRequest() {
    s_manager.unqueue(this);
}

void URLRequest::setHeader(const string &headerName, const string &headerValue) {
    _headers[headerName] = headerValue;
}
void URLRequest::handle(std::function<void (URLRequest*, Status status, const Response&)> handler) {
    _handler = handler;
}

void URLRequest::start() {
}
void URLRequest::dispatch() {
    if (_handler && _status != Cancelled) {
        _handler(this, _status, _response);
    }
    s_manager.complete(this);
    // release();
}

void URLRequest::processResponse() {
    bool noDispatch = false;
    
    bool didError = (_httpStatus>=400) || _status==Status::Error;
    
    // NB: we are on a background thread here. The handler and the release() on this object
    // must be called on the main thread.
    
    s_manager._cache.updateItem(sha1(_url), 0, _response.data);
    
    // Give app code a chance to process the response in the background.
    if (customDecoder) {
        customDecoder(_response);
    } else {
            
        // Default decoding
        if (!didError) {
            string contentType;
            const auto& contentTypeIt = _response.headers.find("content-type");
            if (contentTypeIt != _response.headers.end()) {
                contentType = contentTypeIt->second;
            }
            
            // Bitmaps
            if (contentType.hasPrefix("image/") && _response.data.length()) {
                noDispatch = true; // cos bitmap decode happens on another background thread.
                Bitmap* bitmap = Bitmap::createFromData(_response.data);
                // this runs on main thread
                _response.data.clear(); // don't need this anymore.
                if (_status != Cancelled) {
                    _response.decodedBitmap = bitmap;
                }
                dispatch();
            }

            // JSON
            else if (contentType.contains("json")) {
                string str = _response.data.toString();
                _response.decodedJson = variant::parse(str, PARSEFLAG_JSON);
                _response.data.clear();
            }
            
            // Other text form
            else if (contentType.hasPrefix("text/")) {
                // TODO: Handle charset encodings here. At present we just presume UTF-8.
                _response.decodedText = _response.data.toString();
            }
        }
    }
    // Unless we went via some other background thread, dispatch the result to the main thread.
    if (!noDispatch) {
        App::postToMainThread([=]() {
            dispatch();
        });
    }

}



URLCache::URLCache() {
    
    Task::enqueue({
        {Task::IO, [=](variant&) -> variant {
            return app->fileLoadSync("//cache/url/index.dat");
        }},
        {Task::Background, [=](variant& v) -> variant {
            if (v.isByteArray()) {
                bytestream strm(v.bytearrayRef());
                while (strm.hasMoreToRead()) {
                    pair<sha1_t, item> e;
                    strm.readBytes(sizeof(e.first), &e.first);
                    strm.readBytes(sizeof(e.second.downloadTime), &e.second.downloadTime);
                    strm.readBytes(sizeof(e.second.expiryTime), &e.second.expiryTime);
                    strm.read(e.second.etag);
                    auto p = _map.emplace(e);
                    const pair<sha1_t, item>& pp = *p.first;
                    _list.push_back(&pp);
                }
                v = true;
            }
            _hasLoaded = true;
            return v;
        }}
    });
}

void URLCache::updateItem(const sha1_t& hash, TIMESTAMP expiryTime, const bytearray& data) {
    auto it = _map.find(hash);
}
void URLCache::save() {
    
}
