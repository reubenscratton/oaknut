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
 * @brief General purpose async HTTP networking support.
 * Simple example:\n
 * ````\n
 * URLRequest::get("http://www.foo.com/bar.json")\n
 *    ->handle([=](URLResponse* res) {\n
 *        // handle the json here\n
 *    });\n
 * ````\n
 * When you create a URLRequest it will start automatically on the next tick of the event loop.
 */
class URLRequest : public Object {
public:
    
    /** @name Instantiation
     * @{
     */
    
    /** Create and return a GET request */
    static URLRequest* get(const string& url, Object* owner=nullptr, int flags=0);

    /** Create and return a POST request */
    static URLRequest* post(const string& url, const bytearray& body, Object* owner=nullptr);

    /** Create and return a PATCH request */
    static URLRequest* patch(const string& url, const bytearray& body, Object* owner=nullptr);
    
    /** Create and return a request */
    static URLRequest* createAndStart(const string& url, const string& method,
            const bytearray& body, Object* owner=nullptr, int flags=0);
    
    /**@}*/

    
    /** @name Configuration
     * @{
     */
    /** Set an HTTP header. Will overwrite existing value. */
    void setHeader(const string& headerName, const string& headerValue);
    /**@}*/

    enum CachePolicy {
        Default, // i.e. Conventional HTTP: Use cache value if it's present and still valid, otherwise use remote
        CacheOnly,
        RemoteOnly,
        CacheAndRemote
    };
    void setCachePolicy(CachePolicy policy);
    

    enum Status {
        Error=-1,
        Queued=0,
        Loading,
        LoadedFromCache,
        LoadedFromRemote,
        Cancelled
    };

    

    /** @name Response handling
     * @{
     */
    void handle(std::function<void(const class URLResponse* res, bool isFromCache)> handler);
    
    /** Decode the response on a background thread. Return true to prevent default decoding. */
    std::function<bool(class URLResponse*)> customDecoder;
    
    /**@}*/

    
    
    /** @name Cancellation
     * @{
     */
    /** Cancels the request, ensuring that response handlers won't run. */
    virtual void cancel();
    
    bool isCancelled() const;

    /**@}*/
    
protected:

    URLRequest(const string& url, const string& method, const bytearray& body, Object* owner, int flags);
    ~URLRequest();
    
    virtual void start();
    virtual void processRawResponse(URLResponse* response);
    virtual void dispatchResponse(const URLResponse* response, bool isFromCache);

    // Platform-implemented methods
    error ioLoadRemote(URLResponse* response); // runs on an IO thread, ie should block

    // General data
    Status _status;
    int _flags;
    CachePolicy _cachePolicy;
    std::function<void(const URLResponse*, bool)> _handler;
    sp<Task> _cacheTask;
    sp<Task> _remoteTask;
    os_sem _sem; // Signalled by cancel(). IO task should wait on or poll this to know if request cancelled. 
    std::atomic<bool> _remoteLoadComplete;
    sp<Object> _owner;

    // Request data
    string _url;
    string _method;
    map<string,string> _headers;
    bytearray _body;
    
    
    
    friend class URLCache;
};

class URLResponse : public Object {
public:
    int httpStatus;
    map<string,string> headers;
    
    // Raw bytes. This may be zero length if the
    // response has been decoded.
    bytearray data;
    
    // Decoded forms. At most one of these will be set.
    struct {
        string text;
        variant json;
        sp<class Bitmap> bitmap;
    } decoded;
};

