//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>



static MruCache<string> s_urldataCache(4*1024*1024);


string urlEncode(string str) {
	string rv;
	while (str.length() > 0) {
		int span = (int)strcspn(str.data(), " :/?#[]@!$&'()*+,;=");
		rv = str.substr(0, span);
		str.erase(0, span);
		if (str.length() > 0) {
			char ch = str.charAt(0);
			str.erase(0,1);
			char fmt[8];
			sprintf(fmt, "%%%02X", ch);
			rv.append(fmt);
		}
	}
	return rv;
}

URLRequest::URLRequest(const string& url, const string& method, const string& body, int flags) : _url(url) {
	_method = method;
    _body = body;
    _flags = flags;
}

URLRequest* URLRequest::createAndStart(const string& url, const string& method, const string& body, int flags) {
    auto req = create(url, method, body, flags);
    req->retain(); // keep request alive until run() completes async
    Task::nextTick([=]() {
        req->run();
    });
    return req;
}
URLRequest* URLRequest::get(const string& url, int flags/*=0*/) {
    return createAndStart(url, "GET", "", flags);
}
URLRequest* URLRequest::post(const string& url, const string& body) {
    return createAndStart(url, "POST", body, 0);
}
URLRequest* URLRequest::patch(const string& url, const string& body) {
    return createAndStart(url, "PATCH", body, 0);
}

URLRequest::~URLRequest() {
}

void URLRequest::setHeader(const string &headerName, const string &headerValue) {
    _headers[headerName] = headerValue;
}

void URLRequest::handleData(std::function<void (int httpStatus, ByteBuffer *)> handler) {
    _handlerData = handler;
}

void URLRequest::handleJson(std::function<void (int httpStatus, const Variant &)> handler) {
    _handlerJson = handler;
}

void URLRequest::handleBitmap(std::function<void (int httpStatus, Bitmap *)> handler) {
    _handlerBitmap = handler;
}

void URLRequest::dispatchResult(int httpStatus, const map<string, string>& responseHeaders, ByteBuffer* data) {
    const auto& contentType = responseHeaders.find("content-type");
    if (_handlerBitmap) {
        if (httpStatus >= 400) {
            if (!_cancelled) {
                _handlerBitmap(httpStatus, NULL);
            }
        } else {
            if (contentType != responseHeaders.end()) {
                if (contentType->second == "image/jpeg" || contentType->second == "image/png") {
                    retain();
                    data->retain();
                    Bitmap::createFromData(data->data, (int)data->cb, [=](Bitmap* bitmap) {
                        if (!_cancelled) {
                            _handlerBitmap(httpStatus, bitmap);
                        }
                        data->release();
                        release();
                    });
                } else {
                    app.warn("Unexpected bitmap type %s", contentType->second.data());
                }
            }
        }
    }
    if (_handlerJson) {
        if (httpStatus >= 400) {
            if (!_cancelled) {
                _handlerJson(httpStatus, Variant());
            }
        } else {
            retain();
            data->retain();
            string str = data->toString(true);
            StringProcessor it(str);
            Variant json = Variant::parse(it, 0);
            Task::nextTick([=]() {
                if (!_cancelled) {
                    _handlerJson(httpStatus, json);
                }
                data->release();
                release();
            });
        }
    }
    
    if (_handlerData) {
        if (httpStatus >= 400) {
            if (!_cancelled) {
                _handlerData(httpStatus, NULL);
            }
        } else if (data) {
            retain();
            data->retain();
            Task::nextTick([=]() {
                if (!_cancelled) {
                    _handlerData(httpStatus, data);
                }
                data->release();
                release();
            });
        }
    }
    release();
}


/*void URLRequest::request(const string& url, IURLRequestDelegate* delegate, int flags) {

    URLData* cachedData = NULL;
    if (s_urldataCache.get(url, (Object**)&cachedData)) {
        delegate->onUrlRequestLoad(cachedData);
        return;
    }

	URLRequest* req = NULL;
	map<string, ObjPtr<URLRequest>>::iterator it = s_reqs.find(url);
	if (it == s_reqs.end()) {
		req = new URLRequest(url, delegate, flags);
		s_reqs[url] = req;
	} else {
		req = it->second;
        if (!req) {
            req = new URLRequest(url, delegate, flags);
            s_reqs[url] = req;
        } else {
            req->_delegates.push_back(delegate);
        }
	}

	if (req->_status == IDLE) {
		req->_status = QUEUED;
		s_workQueue.push_back(req);
	}
	flushWorkQueue();
	

}

void URLRequest::removeDelegate(IURLRequestDelegate* delegate) {

	for (int i=0 ; i<_delegates.size() ; i++) {
		if (_delegates.at(i) == delegate) {
			_delegates.erase(_delegates.begin() + i);
			i--;
		}
	}

	if (_delegates.size() == 0) {
		stop();
		s_reqs.erase(_url); // CAUTION! Likely to 'delete this'.
	}
}

void URLRequest::unrequest(const string& url, IURLRequestDelegate* delegate) {
	URLRequest* req = s_reqs[url];
	if (req) {
		req->removeDelegate(delegate);
	}
}



void URLRequest::dispatchOnLoad(URLData* data) {
	if (data) {
        
        // Put in L1 cache
        size_t cb = 0;
        if (data->_type == URLDataTypeBitmap) {
            cb = data->_value.bitmap->sizeInBytes();
        } else {
            cb = data->_value.data->cb;
        }
        s_urldataCache.put(_url, data, cb);
        
        // Fire delegates
		for (int i=0 ; i<_delegates.size() ; i++) {
			IURLRequestDelegate* d = _delegates.at(i);
			d->onUrlRequestLoad(data);
		}
	}
	
	// Was active, now idle
	stop();
	
}*/



