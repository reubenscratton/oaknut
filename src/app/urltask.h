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
 * URLTask::get("http://www.foo.com/bar.json")\n
 *    ->handle([=](URLResponse* res) {\n
 *        // handle the json here\n
 *    });\n
 * ````\n
 * When you create a URLTask it will start automatically on the next tick of the event loop.
 */
class URLTask : public Task {
public:
    
    /** @name Instantiation
     * @{
     */
    
    /** Create and return a GET request */
    static URLTask* get(const string& url, Object* owner=nullptr, int flags=0);

    /** Create and return a POST request */
    static URLTask* post(const string& url, const bytearray& body, Object* owner=nullptr);

    /** Create and return a PATCH request */
    static URLTask* patch(const string& url, const bytearray& body, Object* owner=nullptr);
    
    /** Create and return a URLTask which can be further configured before it starts on the next tick */
    static URLTask* create(const string& url, const string& method,
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
    


    /** @name Response handling
     * @{
     */
    void handle(std::function<void(const class URLResponse* res, bool isFromCache)> handler);
    
    /** Decode the response on a background thread. Return true to prevent default decoding. */
    std::function<bool(class URLResponse*)> customDecoder;
    
    /**@}*/

    
    void cancel() override;

    
protected:

    URLTask(const string& url, const string& method, const bytearray& body, Object* owner, int flags);
    ~URLTask();
    
    virtual void processRawResponse(URLResponse* response);
    virtual void dispatchResponse(const URLResponse* response, bool isFromCache);
    


    // Platform-implemented methods
    error ioLoadRemote(); // runs on an IO thread, ie should block

    // General data
    int _flags;
    CachePolicy _cachePolicy;
    std::function<void(const URLResponse*, bool)> _handler;
    //sp<Task> _cacheTask;
    //sp<Task> _remoteTask;
    semaphore _sem; // Signalled if request cancelled
    std::atomic<bool> _remoteLoadComplete;

    // Request data
    string _url;
    string _method;
    map<string,string> _headers;
    bytearray _body;
  
    // Responses
    sp<class URLResponse> _cachedResponse;
    sp<URLResponse> _remoteResponse;
    
private:
    void startRemoteLoad(const sha1_t& sha);
};

class URLResponse : public Object {
public:
    
    // Raw response data
    int httpStatus;
    map<string,string> headers;
    TIMESTAMP downloadTime;
    TIMESTAMP expiryTime;
    bytearray data; // NB: May be reset to zero length after decoding
    
    // Decoded form
    variant decoded;
    bool hasBeenDecoded;

    string getHeader(const string& headerName);
    virtual uint32_t getRamCost() const override;
};

