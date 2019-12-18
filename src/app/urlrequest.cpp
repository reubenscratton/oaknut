//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



//static MruCache<string> s_urldataCache(4*1024*1024);


URLRequest::URLRequest(const string& url, const string& method, const bytearray& body, int flags) : _url(url) {
	_method = method;
    _body = body;
    _flags = flags;
}

URLRequest* URLRequest::createAndStart(const string& url, const string& method, const bytearray& body, int flags) {
    auto req = create(url, method, body, flags);
    req->retain(); // keep request alive until run() completes async
    App::postToMainThread([=]() {
        req->run();
    });
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
}

void URLRequest::setHeader(const string &headerName, const string &headerValue) {
    _headers[headerName] = headerValue;
}

void URLRequest::handleData(std::function<void (URLRequest*, const bytearray&)> handler) {
    _handlerData = handler;
}

void URLRequest::handleJson(std::function<void (URLRequest*, const variant &)> handler) {
    _handlerJson = handler;
}

void URLRequest::handleBitmap(std::function<void (URLRequest*, Bitmap *)> handler) {
    _handlerBitmap = handler;
}

void URLRequest::dispatchResult(int httpStatus, const map<string, string>& responseHeaders) {
    _httpStatus = httpStatus;
    // NB: we are on a background thread here. The handlers must be called on main thread.
    
    // Give app code a chance to process the data in the background
    if (onGotResponseInBackground && onGotResponseInBackground(httpStatus, responseHeaders)) {
    } else {        
    
        string contentType;
        const auto& contentTypeIt = responseHeaders.find("content-type");
        if (contentTypeIt != responseHeaders.end()) {
            contentType = contentTypeIt->second;
        }
        if (_handlerBitmap) {
            if (didError()) {
                App::postToMainThread([=]() {
                    if (!_cancelled) {
                        _handlerBitmap(this, NULL);
                    }
                });
            } else {
                if (contentType == "image/jpeg" || contentType == "image/png") {
                    Bitmap::createFromData(_responseData, [=](Bitmap* bitmap) {
                        // this runs on main thread
                        if (!_cancelled) {
                            _handlerBitmap(this, bitmap);
                        }
                    });
                } else {
                    app->warn("Unexpected bitmap type %s", contentType.c_str());
                }
            }
        }
        if (contentType.contains("json")) {
            variant json;
            string str = _responseData.toString();
            json = variant::parse(str, PARSEFLAG_JSON);
            if (onGotJsonInBackground) {
                onGotJsonInBackground(json);
            } else {
                if (_handlerJson) {
                    App::postToMainThread([=]() {
                        if (!_cancelled) {
                            _handlerJson(this, json);
                        }
                    });
                }
            }
        }

        if (_handlerData) {
            if (didError()) {
                if (!_cancelled) {
                    App::postToMainThread([=]() {
                        _handlerData(this, _responseData);
                    });
                }
            } else {
                App::postToMainThread([=]() {
                    if (!_cancelled) {
                        _handlerData(this, _responseData);
                    }
                });
            }
        }
    }
    
    App::postToMainThread([=]() {
        release();
    });

}

bool URLRequest::didError() {
    return _httpStatus==0 || _httpStatus>=400;
}
