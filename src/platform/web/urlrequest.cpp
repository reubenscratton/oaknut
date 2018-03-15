//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_WEB

#include <oaknut.h>
#include "bitmap.h"


class NativeRequest : public Object {
public:
    URLRequest* _req;
    val _val;

    NativeRequest(URLRequest* req) : _val(val::null()) {
        _req = req;
    }
    
    static void OnProgress(NativeRequest* req, int done, int total, int timestamp) {
        //printf("progress %d/%d\n", done, total);
    }
    static void OnError(NativeRequest* req) {
        printf("error loading url: %s\n", req->_req->_url.data());
        req->_req->dispatchOnLoad(NULL);
    }
    static void OnImageLoad(NativeRequest* req) {
        app.log("OnImageLoad %s", req->_req->_url.data());
        bool isPng = string::npos!=req->_req->_url.find(".png", 0);
        ObjPtr<OSBitmap> bitmap = new OSBitmap(req->_val, isPng);
        req->_val = val::null();
        req->_req->dispatchOnLoad(new URLData(bitmap));
    }
    static void OnDone(NativeRequest* req, uint8_t* data, int data_size, int timestamp) {
        //printf("done data_size:%d\n", data_size);
        ObjPtr<Data> emdata = new Data();
        emdata->data = (uint8_t*)malloc(data_size);
        memcpy(emdata->data, data, data_size);
        emdata->cb = data_size;
        req->_req->dispatchOnLoad(new URLData(emdata));
    }

    void start() {
        _req->retain();
    
        if (_req->_flags & URL_FLAG_BITMAP) {
        
            // Create an Image
            _val = val::global("Image").new_();
            val got = val::global("GlobalObjectTracker");
            got.set(1, _val);
            
            EM_ASM_ ({
                var req=$0;
                var url=Pointer_stringify($1);
                var onImageLoad=$2;
                var onError=$3;
                var img = GlobalObjectTracker[1];
                img.onload = function() {
                    Runtime.dynCall('vi', onImageLoad, [req]);
                };
                img.onerror = function() {
                    Runtime.dynCall('vi', onError, [req]);
                };
                img.crossOrigin = 'Anonymous';
                img.src = url;
            }, this, _req->_url.data(), OnImageLoad, OnError);
        } else {
    
            // Create an XMLHttpRequest
            _val = val::global("XMLHttpRequest").new_();
            val got = val::global("GlobalObjectTracker");
            got.set(1, _val);

            EM_ASM_ ({
                var req=$0;
                var url=Pointer_stringify($1);
                var onprogress=$2;
                var onload=$3;
                var onerror=$4;
                var xhr = GlobalObjectTracker[1];
                xhr.open("GET", url, true);
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
                            Runtime.dynCall('viiii', onprogress, [req, e.loaded, e.total, date]);
                        }
                            break;
                    }
                };
                
                // done
                xhr.onload=function http_onload(e) {
                    switch(xhr.status) {
                        case 200:
                        case 206:
                        case 300:
                        case 301:
                        case 302:
                            // Bloody CORS screws this
                            /*
                             var content_length=xhr.getResponseHeader('Content-Length');
                             if(content_length===null)content_length=-1; // needed for HEAD requests
                             var content_range =xhr.getResponseHeader('Content-Range'); var cr=-1;
                             if(content_range!=null) {
                             var i=content_range.indexOf('/');
                             if(i>=0)cr=parseInt(content_range.substr(i+1));
                             }*/
                            var date=xhr.getResponseHeader('Last-Modified');
                            date=((date!=null) ? new Date(date).getTime()/1000 : 0);
                            var byteArray=new Uint8Array(xhr.response);
                            var buffer=_malloc(byteArray.length);
                            HEAPU8.set(byteArray, buffer);
                            Runtime.dynCall('viiii', onload, [req, buffer, byteArray.length, date]);
                            _free(buffer);
                            break;
                            
                        default:
                            Runtime.dynCall('vi', onerror, [req]);
                            break;
                    }
                };
                
                // error
                xhr.onerror=function http_onerror(e) {
                    Runtime.dynCall('vi', onerror, [req]);
                };
                
                // limit the number of redirections
                try{if(xhr.channel instanceof Ci.nsIHttpChannel)xhr.channel.redirectionLimit=0;}catch(ex){}
                
                // send
                xhr.send(null);
            }, this, _req->_url.data(), OnProgress, OnDone, OnError);
        }
    }
        
        
    void stop() {
        if (!_val.isNull()) {
            if (_req->_flags & URL_FLAG_BITMAP) {
                _val.set("onload", val::null());
                _val.set("onerror", val::null());
                //_val.set("src", val("data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7"));
            } else {
                _val.call<void>("abort");
            }
            _val = val::null();
        }
        _req->release();
    }
        
    
};

void URLRequest::nativeStart() {
    NativeRequest* nativeReq = new NativeRequest(this);
    nativeReq->retain();
    _osobj = nativeReq;
    nativeReq->start();
}

void URLRequest::nativeStop() {
    NativeRequest* nativeReq = (NativeRequest*)_osobj;
    nativeReq->stop();
    nativeReq->release();
}

#endif
