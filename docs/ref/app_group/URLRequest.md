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
## Static creators

| `<a href="class_u_r_l_request.html">URLRequest</a> * get(const <a href="classstring.html">string</a> & url, int flags)` | Create and return a GET request. |
| `<a href="class_u_r_l_request.html">URLRequest</a> * post(const <a href="classstring.html">string</a> & url, const <a href="classbytearray.html">bytearray</a> & body)` | Create and return a POST request. |
| `<a href="class_u_r_l_request.html">URLRequest</a> * patch(const <a href="classstring.html">string</a> & url, const <a href="classbytearray.html">bytearray</a> & body)` | Create and return a PATCH request. |
| `<a href="class_u_r_l_request.html">URLRequest</a> * createAndStart(const <a href="classstring.html">string</a> & url, const <a href="classstring.html">string</a> & method, const <a href="classbytearray.html">bytearray</a> & body, int flags)` | Create and return a request. |


## Handling response data

| `void handleData(std::function< void(<a href="class_u_r_l_request.html">URLRequest</a> *req)> handler)` |  |
| `void handleJson(std::function< void(<a href="class_u_r_l_request.html">URLRequest</a> *req, const <a href="classvariant.html">variant</a> &)> handler)` |  |
| `void handleBitmap(std::function< void(<a href="class_u_r_l_request.html">URLRequest</a> *req, <a href="class_bitmap.html">Bitmap</a> *)> handler)` |  |


## 



## 

| `void setHeader(const <a href="classstring.html">string</a> & headerName, const <a href="classstring.html">string</a> & headerValue)` |  |
| `void cancel()` |  |
| `bool error()` |  |
| `int getHttpStatus()` |  |
| `const <a href="classbytearray.html">bytearray</a> & getResponseData()` |  |


## 

| `void run()` |  |
| ` URLRequest(const <a href="classstring.html">string</a> & url, const <a href="classstring.html">string</a> & method, const <a href="classbytearray.html">bytearray</a> & body, int flags)` |  |
| ` ~URLRequest()` |  |
| `void dispatchResult(int httpStatus, const map< <a href="classstring.html">string</a>, <a href="classstring.html">string</a> > & responseHeaders)` |  |


## 

| `<a href="class_u_r_l_request.html">URLRequest</a> * create(const <a href="classstring.html">string</a> & url, const <a href="classstring.html">string</a> & method, const <a href="classbytearray.html">bytearray</a> & body, int flags)` |  |


# Methods

| *get* |  `<a href="class_u_r_l_request.html">URLRequest</a> * <a href="todo">get</a>(const <a href="classstring.html">string</a> & url, int flags)` |  |
| *post* |  `<a href="class_u_r_l_request.html">URLRequest</a> * <a href="todo">post</a>(const <a href="classstring.html">string</a> & url, const <a href="classbytearray.html">bytearray</a> & body)` |  |
| *patch* |  `<a href="class_u_r_l_request.html">URLRequest</a> * <a href="todo">patch</a>(const <a href="classstring.html">string</a> & url, const <a href="classbytearray.html">bytearray</a> & body)` |  |
| *createAndStart* |  `<a href="class_u_r_l_request.html">URLRequest</a> * <a href="todo">createAndStart</a>(const <a href="classstring.html">string</a> & url, const <a href="classstring.html">string</a> & method, const <a href="classbytearray.html">bytearray</a> & body, int flags)` |  |
| *handleData* |  `void <a href="todo">handleData</a>(std::function< void(<a href="class_u_r_l_request.html">URLRequest</a> *req)> handler)` |  |
| *handleJson* |  `void <a href="todo">handleJson</a>(std::function< void(<a href="class_u_r_l_request.html">URLRequest</a> *req, const <a href="classvariant.html">variant</a> &)> handler)` |  |
| *handleBitmap* |  `void <a href="todo">handleBitmap</a>(std::function< void(<a href="class_u_r_l_request.html">URLRequest</a> *req, <a href="class_bitmap.html">Bitmap</a> *)> handler)` |  |
| *setHeader* |  `void <a href="todo">setHeader</a>(const <a href="classstring.html">string</a> & headerName, const <a href="classstring.html">string</a> & headerValue)` |  |
| *cancel* |  `void <a href="todo">cancel</a>()` |  |
| *error* |  `bool <a href="todo">error</a>()` |  |
| *getHttpStatus* |  `int <a href="todo">getHttpStatus</a>()` |  |
| *getResponseData* |  `const <a href="classbytearray.html">bytearray</a> & <a href="todo">getResponseData</a>()` |  |
| *run* |  `void <a href="todo">run</a>()` |  |
| *URLRequest* |  ` <a href="todo">URLRequest</a>(const <a href="classstring.html">string</a> & url, const <a href="classstring.html">string</a> & method, const <a href="classbytearray.html">bytearray</a> & body, int flags)` |  |
| *~URLRequest* |  ` <a href="todo">~URLRequest</a>()` |  |
| *dispatchResult* |  `void <a href="todo">dispatchResult</a>(int httpStatus, const map< <a href="classstring.html">string</a>, <a href="classstring.html">string</a> > & responseHeaders)` |  |
| *create* |  `<a href="class_u_r_l_request.html">URLRequest</a> * <a href="todo">create</a>(const <a href="classstring.html">string</a> & url, const <a href="classstring.html">string</a> & method, const <a href="classbytearray.html">bytearray</a> & body, int flags)` |  |
