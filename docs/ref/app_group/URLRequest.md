---
layout: default
---

# URLRequest

```
class URLRequest
    : public Object
```


General purpose async networking support.     

Simple example:  URLRequest::get("http://www.foo.com/bar.json") ->handleJson([=](Variant* json) { // handle the json here });  When you create a URLRequest it will begin, at the earliest, in the next event loop.     
### Static creators

[`URLRequest`](/oaknut/ref/app_group/URLRequest)` * get(const `[`string`](/oaknut/ref/base_group/string)` & url, int flags)`<br>Create and return a GET request.

[`URLRequest`](/oaknut/ref/app_group/URLRequest)` * post(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body)`<br>Create and return a POST request.

[`URLRequest`](/oaknut/ref/app_group/URLRequest)` * patch(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body)`<br>Create and return a PATCH request.

[`URLRequest`](/oaknut/ref/app_group/URLRequest)` * createAndStart(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`string`](/oaknut/ref/base_group/string)` & method, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body, int flags)`<br>Create and return a request.



### Handling response data

`void handleData(std::function< void(`[`URLRequest`](/oaknut/ref/app_group/URLRequest)` *req)> handler)`<br>

`void handleJson(std::function< void(`[`URLRequest`](/oaknut/ref/app_group/URLRequest)` *req, const variant &)> handler)`<br>

`void handleBitmap(std::function< void(`[`URLRequest`](/oaknut/ref/app_group/URLRequest)` *req, `[`Bitmap`](/oaknut/ref/graphics_group/Bitmap)` *)> handler)`<br>



`void setHeader(const `[`string`](/oaknut/ref/base_group/string)` & headerName, const `[`string`](/oaknut/ref/base_group/string)` & headerValue)`<br>

`void cancel()`<br>

`bool error()`<br>

`int getHttpStatus()`<br>

`const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & getResponseData()`<br>



`void run()`<br>

` URLRequest(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`string`](/oaknut/ref/base_group/string)` & method, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body, int flags)`<br>

` ~URLRequest()`<br>

`void dispatchResult(int httpStatus, const map< `[`string`](/oaknut/ref/base_group/string)`, `[`string`](/oaknut/ref/base_group/string)` > & responseHeaders)`<br>



[`URLRequest`](/oaknut/ref/app_group/URLRequest)` * create(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`string`](/oaknut/ref/base_group/string)` & method, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body, int flags)`<br>



## Methods

| | |
|-|-|
| *URLRequest* | ` URLRequest(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`string`](/oaknut/ref/base_group/string)` & method, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body, int flags)` |  |
| *cancel* | `void cancel()` |  |
| *create* | [`URLRequest`](/oaknut/ref/app_group/URLRequest)` * create(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`string`](/oaknut/ref/base_group/string)` & method, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body, int flags)` |  |
| *createAndStart* | [`URLRequest`](/oaknut/ref/app_group/URLRequest)` * createAndStart(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`string`](/oaknut/ref/base_group/string)` & method, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body, int flags)` |  |
| *dispatchResult* | `void dispatchResult(int httpStatus, const map< `[`string`](/oaknut/ref/base_group/string)`, `[`string`](/oaknut/ref/base_group/string)` > & responseHeaders)` |  |
| *error* | `bool error()` |  |
| *get* | [`URLRequest`](/oaknut/ref/app_group/URLRequest)` * get(const `[`string`](/oaknut/ref/base_group/string)` & url, int flags)` |  |
| *getHttpStatus* | `int getHttpStatus()` |  |
| *getResponseData* | `const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & getResponseData()` |  |
| *handleBitmap* | `void handleBitmap(std::function< void(`[`URLRequest`](/oaknut/ref/app_group/URLRequest)` *req, `[`Bitmap`](/oaknut/ref/graphics_group/Bitmap)` *)> handler)` |  |
| *handleData* | `void handleData(std::function< void(`[`URLRequest`](/oaknut/ref/app_group/URLRequest)` *req)> handler)` |  |
| *handleJson* | `void handleJson(std::function< void(`[`URLRequest`](/oaknut/ref/app_group/URLRequest)` *req, const variant &)> handler)` |  |
| *patch* | [`URLRequest`](/oaknut/ref/app_group/URLRequest)` * patch(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body)` |  |
| *post* | [`URLRequest`](/oaknut/ref/app_group/URLRequest)` * post(const `[`string`](/oaknut/ref/base_group/string)` & url, const `[`bytearray`](/oaknut/ref/base_group/bytearray)` & body)` |  |
| *run* | `void run()` |  |
| *setHeader* | `void setHeader(const `[`string`](/oaknut/ref/base_group/string)` & headerName, const `[`string`](/oaknut/ref/base_group/string)` & headerValue)` |  |
| *~URLRequest* | ` ~URLRequest()` |  |
