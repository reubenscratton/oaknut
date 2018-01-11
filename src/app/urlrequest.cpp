//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "../oaknut.h"

URLData::URLData(Data* data) : _value(data), _type(URLDataTypeData) {}
URLData::URLData(JsonValue* json) : _value(json), _type(URLDataTypeJson) {}
URLData::URLData(Bitmap* bitmap) : _value(bitmap), _type(URLDataTypeBitmap) {}
URLData::~URLData() {
    if (_type == URLDataTypeBitmap) {
        _value.bitmap->release();
    } else if (_type == URLDataTypeData) {
        _value.data->release();
    }
}

static MruCache<string> s_urldataCache(4*1024*1024);
static list<ObjPtr<URLRequest>> s_workQueue;
static ObjPtr<URLRequest> s_currentReq;
static map<string, ObjPtr<URLRequest>> s_reqs;

void URLRequest::flushWorkQueue() {
	if (s_workQueue.size() == 0) {
		return;
	}
	if (s_currentReq != NULL) {
		return;
	}
	s_currentReq = s_workQueue.front();
	s_workQueue.pop_front();
	s_currentReq->start();
}

void URLRequest::start() {
	assert(!_osobj);
	_status = RUNNING;
	retain(); // ensure urlrequest is alive for duration of network request
    //oakLog("starting %s", _url.data());
    nativeStart();
	assert(_osobj);
}

void URLRequest::stop() {
	if (_osobj) {
		nativeStop();
		_osobj = NULL;
		_status = IDLE;
		if (this == s_currentReq) {
			s_currentReq = NULL;
		}
		flushWorkQueue();
		release();
	}
}



string urlEncode(string str) {
	string rv;
	while (str.length() > 0) {
		size_t span = strcspn(str.data(), " :/?#[]@!$&'()*+,;=");
		rv.append(str, 0, span);
		str.erase(0, span);
		if (str.length() > 0) {
			char ch = str.at(0);
			str.erase(0,1);
			char fmt[8];
			sprintf(fmt, "%%%02X", ch);
			rv.append(fmt);
		}
	}
	return rv;
}

URLRequest::URLRequest(const string& url, IURLRequestDelegate* delegate, int flags) : _url(url)
{
	_status = IDLE;
	_method = "GET";
	_delegates.push_back(delegate);
    _flags = flags;
}
URLRequest::~URLRequest() {
	//oakLog("~URLRequest %lX", (int64_t)this);
	assert(_delegates.size()==0);
}

void URLRequest::request(const string& url, IURLRequestDelegate* delegate, int flags) {

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
	
}



