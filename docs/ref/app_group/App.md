---
layout: default
---

# App

```
class App
    : public Object
```


Global object accessible through the global `app` variable that provides essential top-level services such as loading resources, persisting configuration settings and many more.     

    
### Entry point

`void main()`<br>Application entry point.
Your implementation of this must instantiate a ViewController and set it as _window.rootViewController before returning


### Time

`TIMESTAMP currentMillis()`<br>Gets the current system time, in milliseconds.

[`string`](/oaknut/ref/base_group/string)` friendlyTimeString(TIMESTAMP timestamp)`<br>Returns a 'friendly' text representation of a timestamp.


### Logging

`void log(char const* fmt, ... )`<br>Log an informational message.

`void warn(char const* fmt, ... )`<br>Log a warning message prefixed with "Warning: ".


### File paths

[`string`](/oaknut/ref/base_group/string)` getPathForGeneralFiles()`<br>
[`string`](/oaknut/ref/base_group/string)` getPathForUserDocuments()`<br>
[`string`](/oaknut/ref/base_group/string)` getPathForCacheFiles()`<br>
[`string`](/oaknut/ref/base_group/string)` getPathForTemporaryFiles()`<br>

### Assets

`class `[`ByteBuffer`](/oaknut/ref/data_group/ByteBuffer)`* loadAsset(const char* assetPath)`<br>Load a file from the assets directory, synchronously.
Since this does IO it's best to limit use to app startup and background threads


### Styles

`void loadStyleAsset(const `[`string`](/oaknut/ref/base_group/string)` & assetPath)`<br>Load a style asset from the assets directory.
Generally used in App::main()

`const `[`StyleValue`](/oaknut/ref/app_group/StyleValue)`* getStyleValue(const `[`string`](/oaknut/ref/base_group/string)` & keypath)`<br>Get a named style value.

[`string`](/oaknut/ref/base_group/string)` getStyleString(const `[`string`](/oaknut/ref/base_group/string)` & key, const char* defaultString)`<br>Get a named style value and coerce to a string.

`float getStyleFloat(const `[`string`](/oaknut/ref/base_group/string)` & key)`<br>Get a named style value and coerce to a float.

[`COLOR`](/oaknut/ref/graphics_group/COLOR)` getStyleColor(const `[`string`](/oaknut/ref/base_group/string)` & key)`<br>Get a named style value and coerce to a COLOR.


### Layout

`class `[`View`](/oaknut/ref/views/View)`* layoutInflate(const `[`string`](/oaknut/ref/base_group/string)` & assetPath)`<br>Loads a view from an asset file.
See the Layout Guide for details

`void layoutInflateExistingView(`[`View`](/oaknut/ref/views/View)`* view, const `[`string`](/oaknut/ref/base_group/string)` & assetPath)`<br>
`float dp(float dp)`<br>Converts a 'dp' measurement into physical pixels.
Dp is a device-independent pixel and is based on 320dpi

`float idp(float pix)`<br>Converts a 'dp' measurement into physical pixels and floor()s the result to an integer.


### Locale

[`string`](/oaknut/ref/base_group/string)` currentCountryCode()`<br>Returns ISO-3166 two-letter country code the device is configured for, which is not necessarily the country the device is currently in.


### Settings

`int getIntSetting(const char* key, const int defaultValue)`<br>Gets a named integer setting.

`void setIntSetting(const char* key, int value)`<br>Sets a named integer setting.

[`string`](/oaknut/ref/base_group/string)` getStringSetting(const char* key, const char* defaultValue)`<br>Gets a named string setting.

`void setStringSetting(const char* key, const char* value)`<br>Sets a named string setting.


### Scheduling Callbacks

[`Task`](/oaknut/ref/base_group/Task)`* postToMainThread(std::function< void(void)> func, int delay)`<br>Schedule a function to run on the main thread loop, optionally after a delay (in milliseconds).


