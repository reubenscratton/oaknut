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
 *    ->handle([=](URLRequest* req, const Response& response) {\n
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
    static URLRequest* get(const string& url, int flags=0);

    /** Create and return a POST request */
    static URLRequest* post(const string& url, const bytearray& body);

    /** Create and return a PATCH request */
    static URLRequest* patch(const string& url, const bytearray& body);
    
    /** Create and return a request */
    static URLRequest* createAndStart(const string& url, const string& method, const bytearray& body, int flags);
    
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

    
    struct Response {
        bool isFromCache;
        TIMESTAMP cacheExpiryTime;
        int _httpStatus;
        map<string,string> headers;
        bytearray data;
        string decodedText;
        variant decodedJson;
        sp<class Bitmap> decodedBitmap;
    };
    
    /** @name Response handling
     * @{
     */
    void handle(std::function<void(URLRequest*, Status status, const Response&)> handler);
    
    /** Decode the response on the downloading thread and maybe dispatch some other kind of handler. */
    std::function<void(Response&)> customDecoder;
    
    /**@}*/

    
    
    /** @name Cancellation
     * @{
     */
    /** Cancels the request, ensuring that response handlers won't run. */
    virtual void cancel() =0;
    /**@}*/
    
protected:
    virtual void start();
    virtual void load() =0;

    URLRequest(const string& url, const string& method, const bytearray& body, int flags);
    ~URLRequest();
    
    // General data
    Status _status;
    int _flags;
    CachePolicy _cachePolicy;
    std::function<void(URLRequest*, Status status, const Response& response)> _handler;

    // Request data
    string _url;
    string _method;
    map<string,string> _headers;
    bytearray _body;
    
    // Response data
    Response _response;
    int _httpStatus;
    
    // Queue data
    list<URLRequest*>::iterator _it;

    static URLRequest* create(const string& url, const string& method, const bytearray& body, int flags);

    virtual void dispatch();
    virtual void processResponse();
    
    friend class URLRequestManager;
    friend class URLCache;
    friend class URLLoader;
};

class URLCache : public Object {
public:

    URLCache();

    struct item {
      TIMESTAMP downloadTime;
      TIMESTAMP expiryTime;
      string etag;
    };

    bool containsItem(const sha1_t& hash, struct item* item);
    void loadItem(const sha1_t& hash, std::function<void(const bytearray&)> callback);
    void updateItem(const sha1_t& hash, TIMESTAMP expiry, const bytearray& data);
    void save();


protected:

    bool _hasLoaded;
    map<sha1_t, item> _map;
    list<const pair<sha1_t, item>*> _list;
};
