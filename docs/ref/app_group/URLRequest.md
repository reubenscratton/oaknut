---
layout: default
---

# URLRequest

```
class URLRequest
    : public Object
```


General purpose async HTTP networking support.     

Simple example:
````
URLRequest::get("http://www.foo.com/bar.json")
 ->handleJson([=](const variant& json) {
 // handle the json here
 });
````
When you create a URLRequest it will start automatically on the next tick of the event loop.     
### Instantiation

[`URLRequest`](/ref/app_group/URLRequest)`* get(const `[`string`](/ref/base_group/string)` & url, int flags)`<br>Create and return a GET request.

[`URLRequest`](/ref/app_group/URLRequest)`* post(const `[`string`](/ref/base_group/string)` & url, const `[`bytearray`](/ref/base_group/bytearray)` & body)`<br>Create and return a POST request.

[`URLRequest`](/ref/app_group/URLRequest)`* patch(const `[`string`](/ref/base_group/string)` & url, const `[`bytearray`](/ref/base_group/bytearray)` & body)`<br>Create and return a PATCH request.

[`URLRequest`](/ref/app_group/URLRequest)`* createAndStart(const `[`string`](/ref/base_group/string)` & url, const `[`string`](/ref/base_group/string)` & method, const `[`bytearray`](/ref/base_group/bytearray)` & body, int flags)`<br>Create and return a request.


### Configuration

`void setHeader(const `[`string`](/ref/base_group/string)` & headerName, const `[`string`](/ref/base_group/string)` & headerValue)`<br>Set an HTTP header.
Will overwrite existing value.


### Handling response data

`void handleData(std::function< void(`[`URLRequest`](/ref/app_group/URLRequest)`*req, const `[`bytearray`](/ref/base_group/bytearray)` &)> handler)`<br>

`void handleJson(std::function< void(`[`URLRequest`](/ref/app_group/URLRequest)`*req, const variant &)> handler)`<br>

`void handleBitmap(std::function< void(`[`URLRequest`](/ref/app_group/URLRequest)`*req, `[`Bitmap`](/ref/graphics_group/Bitmap)`*)> handler)`<br>


### Response status

`bool didError()`<br>

`int getHttpStatus()`<br>

`const `[`bytearray`](/ref/base_group/bytearray)` & getResponseData()`<br>


### Cancellation

`void cancel()`<br>Cancels the request, ensuring that response handlers won't run.


`void run()`<br>

` URLRequest(const `[`string`](/ref/base_group/string)` & url, const `[`string`](/ref/base_group/string)` & method, const `[`bytearray`](/ref/base_group/bytearray)` & body, int flags)`<br>

` ~URLRequest()`<br>

`void dispatchResult(int httpStatus, const map< `[`string`](/ref/base_group/string)`, `[`string`](/ref/base_group/string)` > & responseHeaders)`<br>


[`URLRequest`](/ref/app_group/URLRequest)`* create(const `[`string`](/ref/base_group/string)` & url, const `[`string`](/ref/base_group/string)` & method, const `[`bytearray`](/ref/base_group/bytearray)` & body, int flags)`<br>


