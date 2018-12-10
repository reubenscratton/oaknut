//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


// TODO: remove this flag and let this decision hinge on whether Bitmap handler set or not
#define URL_FLAG_BITMAP 1   // use this when requesting images. Exists so web can use Image rather than XHR

/**
 * @ingroup app_group
 * @brief General purpose async networking support.
 Simple example:
 ````
 URLRequest::get("http://www.foo.com/bar.json")
    ->handleJson([=](Variant* json) {
        // handle the json here
    });
 ````
 When you create a URLRequest it will begin, at the earliest, in the next event loop.
 */
class URLRequest : public Object {
public:
    
    /** @name Static creators
     * @{
     */
    
    /** Create and return a GET request */
    static URLRequest* get(const string& url, int flags=0);

    /** Create and return a POST request */
    static URLRequest* post(const string& url, const bytearray& body);

    /** Create and return a PATCH request */
    static URLRequest* patch(const string& url, const bytearray& body);
    
    /** Create and return a request */
    static URLRequest* createAndStart(const string& url, const string& method, const bytearray& body, int flags);
    
    /**@}*/

    void setHeader(const string& headerName, const string& headerValue);


    /** @name Handling response data
     * @{
     */
    void handleData(std::function<void(URLRequest* req)> handler);
    void handleJson(std::function<void(URLRequest* req, const variant&)> handler);
    void handleBitmap(std::function<void(URLRequest* req, Bitmap*)> handler);
    
    /** Process the response on the downloading thread and maybe dispatch some other kind of handler */
    std::function<bool(int,const map<string, string>&)> onGotResponseInBackground;
    /**@}*/

    virtual void cancel() =0;

    bool error();
    int getHttpStatus() const { return _httpStatus; }
    const bytearray& getResponseData() const { return _responseData; }

    
protected:
    virtual void run() =0;

    URLRequest(const string& url, const string& method, const bytearray& body, int flags);
    ~URLRequest();
    string _url;
    string _method;
    map<string,string> _headers;
    bytearray _body;
    int _httpStatus;
    bytearray _responseData;
    int _flags;
    bool _cancelled;
    std::function<void(URLRequest* req)> _handlerData;
    std::function<void(URLRequest* req, const variant&)> _handlerJson;
    std::function<void(URLRequest* req, Bitmap*)> _handlerBitmap;
    
    static URLRequest* create(const string& url, const string& method, const bytearray& body, int flags);

    virtual void dispatchResult(int httpStatus, const map<string, string>& responseHeaders);
};

