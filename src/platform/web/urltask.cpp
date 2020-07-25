//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>
#include "bitmap.h"


class URLRequestWeb : public URLRequest {
public:
    val _val;

    URLRequestWeb(const string& url, const string& method, const bytearray& body, int flags) : URLRequest(url, method, body, flags), _val(val::null()) {
    }
    
    static void OnProgress(URLRequestWeb* req, int done, int total, int timestamp) {
        //printf("progress %d/%d\n", done, total);
    }
    static void OnError(URLRequestWeb* req) {
        printf("error loading url: %s\n", req->_url.c_str());
        req->_status = Status::Error;
        req->dispatch();
    }
    static void OnImageLoad(URLRequestWeb* req) {
        
        // This is a rather special case. Instead of calling processResponse() we use
        // a special web-only Bitmap constructor and dispatch from here.
        req->_response.httpStatus = 200;
        req->_response.decodedBitmap = new BitmapWeb(req->_val);
        req->_val = val::null();
        req->dispatch();
    }
    static void OnNonImageLoad(URLRequestWeb* req, int httpStatus, uint8_t* data, int data_size, int timestamp, const char* szHeaders) {
        //log_dbg("status %d len=%d", httpStatus, data_size);
        //string foo((const char*)data, data_size);
        //log_dbg("%s", foo.c_str());
        req->_response.httpStatus = httpStatus;
        vector<string> headers = string(szHeaders,-1).split("\r\n");
        for (auto& header: headers) {
            int32_t colonPos = header.find(":");
            if (colonPos>0) {
                string name = header.substr(0, colonPos);
                string value = header.substr(colonPos+1, -1);
                req->_response.headers[name] = value;
            }
        }
        req->_responseData.assignNoCopy(data, data_size);
        req->processResponse();
    }

    virtual void run() {
    
        if (_flags & URL_FLAG_BITMAP) {
        
            // Create an Image
            _val = val::global("Image").new_();
            int gotIndex = val::global("gotSet")(_val).as<int>();
            EM_ASM_ ({
                var req=$0;
                var url=UTF8ToString($1);
                var onImageLoad=$2;
                var onError=$3;
                var img = gotGet($4);
                img.onload = function() {
                    dynCall('vi', onImageLoad, [req]);
                };
                img.onerror = function() {
                    dynCall('vi', onError, [req]);
                };
                img.crossOrigin = 'Anonymous';
                img.src = url;
            }, this, _url.c_str(), OnImageLoad, OnError, gotIndex);
        } else {
    
            // Create an XMLHttpRequest
            _val = val::global("XMLHttpRequest").new_();
            
            // Open it
            _val.call<void>("open", val(_method.c_str()), val(_url.c_str()), val(true));

            // Set headers
            for (auto& header : _headers) {
                const string& headerName = header.first;
                const string& headerValue = header.second;
                _val.call<void>("setRequestHeader", val(headerName.c_str()), val(headerValue.c_str()));
            }

            int gotIndex = val::global("gotSet")(_val).as<int>();
            EM_ASM_ ({
                var req=$0;
                var url=UTF8ToString($1);
                var onprogress=$2;
                var onload=$3;
                var onerror=$4;
                var xhr = gotGet($5);
                xhr.responseType='arraybuffer';
                
                // progress
                xhr.onprogress=function http_onprogress(e) {
                    switch(xhr.status) {
                        case 200:
                        case 206:
                        case 300:
                        case 301:
                        case 302: {
                            var date=xhr.getResponseHeader('Last-Modified');
                            date=((date!=null) ? new Date(date).getTime()/1000 : 0);
                            dynCall('viiii', onprogress, [req, e.loaded, e.total, date]);
                        }
                            break;
                    }
                };
                
                // done
                xhr.onload=function http_onload(e) {
                    switch(xhr.status) {
                        case 200:
                        case 201:
                        case 202:
                        case 206:
                        case 300:
                        case 301:
                        case 302:
                            var headers=xhr.getAllResponseHeaders();
                            var cb=lengthBytesUTF8(headers) + 1;
                            var headersBuff=_malloc(cb);
                            stringToUTF8(headers, headersBuff, cb);

                            var date=xhr.getResponseHeader('Last-Modified');
                            date=((date!=null) ? new Date(date).getTime()/1000 : 0);
                            var byteArray=new Uint8Array(xhr.response);
                            var buffer=_malloc(byteArray.length);
                            HEAPU8.set(byteArray, buffer);
                            dynCall('viiiiii', onload, [req, xhr.status, buffer, byteArray.length, date,headersBuff]);
                            Module._free(headersBuff);
                            break;
                            
                        default:
                            dynCall('vi', onerror, [req]);
                            break;
                    }
                };
                
                // error
                xhr.onerror=function http_onerror(e) {
                    dynCall('vi', onerror, [req]);
                };
                
                // limit the number of redirections
                try{if(xhr.channel instanceof Ci.nsIHttpChannel)xhr.channel.redirectionLimit=0;}catch(ex){}
                
            }, this, _url.c_str(), OnProgress, OnNonImageLoad, OnError, gotIndex);

            // Send
            val body = val::null();
            if (_body.size()) {
                int gotIndex = EM_ASM_INT({
                    return gotSet(new Uint8Array(HEAPU8.buffer, $0, $1));
                }, _body.data(), _body.size());
                body = val::global("gotGet")(gotIndex);
            }
            _val.call<void>("send", body);

        }
    }
        
        
    virtual void cancel() {
        _cancelled = true;
        if (!_val.isNull()) {
            if (_flags & URL_FLAG_BITMAP) {
                _val.set("onload", val::null());
                _val.set("onerror", val::null());
                //_val.set("src", val("data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7"));
            } else {
                _val.call<void>("abort");
            }
            _val = val::null();
        }
    }
        
    
};


URLRequest* URLRequest::create(const string& url, const string& method, const bytearray& body, int flags) {
    return new URLRequestWeb(url, method, body, flags);
}

#endif
