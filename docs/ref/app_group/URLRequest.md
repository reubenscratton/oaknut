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

| | |
|-|-|
|[`URLRequest`](/ref/app_group/URLRequest)` * get(const `[`string`](/ref/base_group/string)` & url, int flags)`|Create and return a GET request.|
|[`URLRequest`](/ref/app_group/URLRequest)` * post(const `[`string`](/ref/base_group/string)` & url, const `[`bytearray`](/ref/base_group/bytearray)` & body)`|Create and return a POST request.|
|[`URLRequest`](/ref/app_group/URLRequest)` * patch(const `[`string`](/ref/base_group/string)` & url, const `[`bytearray`](/ref/base_group/bytearray)` & body)`|Create and return a PATCH request.|
|[`URLRequest`](/ref/app_group/URLRequest)` * createAndStart(const `[`string`](/ref/base_group/string)` & url, const `[`string`](/ref/base_group/string)` & method, const `[`bytearray`](/ref/base_group/bytearray)` & body, int flags)`|Create and return a request.|


### Handling response data

| | |
|-|-|
|`void handleData(std::function< void(`[`URLRequest`](/ref/app_group/URLRequest)` *req)> handler)`||
|`void handleJson(std::function< void(`[`URLRequest`](/ref/app_group/URLRequest)` *req, const variant &)> handler)`||
|`void handleBitmap(std::function< void(`[`URLRequest`](/ref/app_group/URLRequest)` *req, `[`Bitmap`](/ref/graphics_group/Bitmap)` *)> handler)`||


| | |
|-|-|
|`void setHeader(const `[`string`](/ref/base_group/string)` & headerName, const `[`string`](/ref/base_group/string)` & headerValue)`||
|`void cancel()`||
|`bool error()`||
|`int getHttpStatus()`||
|`const `[`bytearray`](/ref/base_group/bytearray)` & getResponseData()`||


| | |
|-|-|
|`void run()`||
|` URLRequest(const `[`string`](/ref/base_group/string)` & url, const `[`string`](/ref/base_group/string)` & method, const `[`bytearray`](/ref/base_group/bytearray)` & body, int flags)`||
|` ~URLRequest()`||
|`void dispatchResult(int httpStatus, const map< `[`string`](/ref/base_group/string)`, `[`string`](/ref/base_group/string)` > & responseHeaders)`||


| | |
|-|-|
|[`URLRequest`](/ref/app_group/URLRequest)` * create(const `[`string`](/ref/base_group/string)` & url, const `[`string`](/ref/base_group/string)` & method, const `[`bytearray`](/ref/base_group/bytearray)` & body, int flags)`||


## Methods

| | |
|-|-|
| *get* | [`URLRequest`](/ref/app_group/URLRequest)` * get(const `[`string`](/ref/base_group/string)` & url, int flags)` |  |
| *post* | [`URLRequest`](/ref/app_group/URLRequest)` * post(const `[`string`](/ref/base_group/string)` & url, const `[`bytearray`](/ref/base_group/bytearray)` & body)` |  |
| *patch* | [`URLRequest`](/ref/app_group/URLRequest)` * patch(const `[`string`](/ref/base_group/string)` & url, const `[`bytearray`](/ref/base_group/bytearray)` & body)` |  |
| *createAndStart* | [`URLRequest`](/ref/app_group/URLRequest)` * createAndStart(const `[`string`](/ref/base_group/string)` & url, const `[`string`](/ref/base_group/string)` & method, const `[`bytearray`](/ref/base_group/bytearray)` & body, int flags)` |  |
| *handleData* | `void handleData(std::function< void(`[`URLRequest`](/ref/app_group/URLRequest)` *req)> handler)` |  |
| *handleJson* | `void handleJson(std::function< void(`[`URLRequest`](/ref/app_group/URLRequest)` *req, const variant &)> handler)` |  |
| *handleBitmap* | `void handleBitmap(std::function< void(`[`URLRequest`](/ref/app_group/URLRequest)` *req, `[`Bitmap`](/ref/graphics_group/Bitmap)` *)> handler)` |  |
| *setHeader* | `void setHeader(const `[`string`](/ref/base_group/string)` & headerName, const `[`string`](/ref/base_group/string)` & headerValue)` |  |
| *cancel* | `void cancel()` |  |
| *error* | `bool error()` |  |
| *getHttpStatus* | `int getHttpStatus()` |  |
| *getResponseData* | `const `[`bytearray`](/ref/base_group/bytearray)` & getResponseData()` |  |
| *run* | `void run()` |  |
| *URLRequest* | ` URLRequest(const `[`string`](/ref/base_group/string)` & url, const `[`string`](/ref/base_group/string)` & method, const `[`bytearray`](/ref/base_group/bytearray)` & body, int flags)` |  |
| *~URLRequest* | ` ~URLRequest()` |  |
| *dispatchResult* | `void dispatchResult(int httpStatus, const map< `[`string`](/ref/base_group/string)`, `[`string`](/ref/base_group/string)` > & responseHeaders)` |  |
| *create* | [`URLRequest`](/ref/app_group/URLRequest)` * create(const `[`string`](/ref/base_group/string)` & url, const `[`string`](/ref/base_group/string)` & method, const `[`bytearray`](/ref/base_group/bytearray)` & body, int flags)` |  |
