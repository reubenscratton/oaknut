---
layout: default
---

# App

```
class App
    : public Object
```


The single instance of this class is accessed through the global 'app' variable.     

    
### Time

| | |
|-|-|
|`TIMESTAMP currentMillis()`|Gets the current system time, in milliseconds.|
|[`string`](/ref/base_group/string)` friendlyTimeString(TIMESTAMP timestamp)`||


### Logging

| | |
|-|-|
|`void log(char const * fmt, ... )`|Log an informational message.|
|`void warn(char const * fmt, ... )`|Log a warning message prefixed with "Warning: ".|


### File paths

| | |
|-|-|
|[`string`](/ref/base_group/string)` getPathForGeneralFiles()`||
|[`string`](/ref/base_group/string)` getPathForUserDocuments()`||
|[`string`](/ref/base_group/string)` getPathForCacheFiles()`||
|[`string`](/ref/base_group/string)` getPathForTemporaryFiles()`||


### Styles

| | |
|-|-|
|`void loadStyleAsset(const `[`string`](/ref/base_group/string)` & assetPath)`|Load a style asset from the assets directory.|
|`const `[`StyleValue`](/ref/app_group/StyleValue)` * getStyleValue(const `[`string`](/ref/base_group/string)` & keypath)`|Get a named style value.|
|[`string`](/ref/base_group/string)` getStyleString(const `[`string`](/ref/base_group/string)` & key, const char * defaultString)`|Get a named style value and coerce to a string.|
|`float getStyleFloat(const `[`string`](/ref/base_group/string)` & key)`|Get a named style value and coerce to a float.|
|[`COLOR`](/ref/graphics_group/COLOR)` getStyleColor(const `[`string`](/ref/base_group/string)` & key)`|Get a named style value and coerce to a COLOR.|


### Layout

| | |
|-|-|
|`class `[`View`](/ref/views/View)` * layoutInflate(const `[`string`](/ref/base_group/string)` & assetPath)`|Loads a view from an asset file.|
|`void layoutInflateExistingView(`[`View`](/ref/views/View)` * view, const `[`string`](/ref/base_group/string)` & assetPath)`||
|`float dp(float dp)`|Converts a 'dp' measurement into physical pixels.|
|`float idp(float pix)`|Converts a 'dp' measurement into physical pixels and floor()s the result to an integer.|


### Locale

| | |
|-|-|
|[`string`](/ref/base_group/string)` currentCountryCode()`|Returns ISO-3166 two-letter country code the device is configured for, which is not necessarily the country the device is currently in.|


### Settings

| | |
|-|-|
|`int getIntSetting(const char * key, const int defaultValue)`|Gets a named integer setting.|
|`void setIntSetting(const char * key, int value)`|Sets a named integer setting.|
|[`string`](/ref/base_group/string)` getStringSetting(const char * key, const char * defaultValue)`|Gets a named string setting.|
|`void setStringSetting(const char * key, const char * value)`|Sets a named string setting.|


| | |
|-|-|
|` App()`||
|`void main()`|Application entry point.|
|`class `[`ByteBuffer`](/ref/data_group/ByteBuffer)` * loadAsset(const char * assetPath)`|Load a file from the assets directory, synchronously.|


| | |
|-|-|
|[`Task`](/ref/base_group/Task)` * postToMainThread(std::function< void(void)> func, int delay)`||


## Methods

| | |
|-|-|
| *currentMillis* | `TIMESTAMP currentMillis()` |  |
| *friendlyTimeString* | [`string`](/ref/base_group/string)` friendlyTimeString(TIMESTAMP timestamp)` |  |
| *log* | `void log(char const * fmt, ... )` |  |
| *warn* | `void warn(char const * fmt, ... )` |  |
| *getPathForGeneralFiles* | [`string`](/ref/base_group/string)` getPathForGeneralFiles()` |  |
| *getPathForUserDocuments* | [`string`](/ref/base_group/string)` getPathForUserDocuments()` |  |
| *getPathForCacheFiles* | [`string`](/ref/base_group/string)` getPathForCacheFiles()` |  |
| *getPathForTemporaryFiles* | [`string`](/ref/base_group/string)` getPathForTemporaryFiles()` |  |
| *loadStyleAsset* | `void loadStyleAsset(const `[`string`](/ref/base_group/string)` & assetPath)` | Generally used in App::main() |
| *getStyleValue* | `const `[`StyleValue`](/ref/app_group/StyleValue)` * getStyleValue(const `[`string`](/ref/base_group/string)` & keypath)` |  |
| *getStyleString* | [`string`](/ref/base_group/string)` getStyleString(const `[`string`](/ref/base_group/string)` & key, const char * defaultString)` |  |
| *getStyleFloat* | `float getStyleFloat(const `[`string`](/ref/base_group/string)` & key)` |  |
| *getStyleColor* | [`COLOR`](/ref/graphics_group/COLOR)` getStyleColor(const `[`string`](/ref/base_group/string)` & key)` |  |
| *layoutInflate* | `class `[`View`](/ref/views/View)` * layoutInflate(const `[`string`](/ref/base_group/string)` & assetPath)` | See the Layout Guide for details |
| *layoutInflateExistingView* | `void layoutInflateExistingView(`[`View`](/ref/views/View)` * view, const `[`string`](/ref/base_group/string)` & assetPath)` |  |
| *dp* | `float dp(float dp)` | Dp is a device-independent pixel and is based on 320dpi |
| *idp* | `float idp(float pix)` |  |
| *currentCountryCode* | [`string`](/ref/base_group/string)` currentCountryCode()` |  |
| *getIntSetting* | `int getIntSetting(const char * key, const int defaultValue)` |  |
| *setIntSetting* | `void setIntSetting(const char * key, int value)` |  |
| *getStringSetting* | [`string`](/ref/base_group/string)` getStringSetting(const char * key, const char * defaultValue)` |  |
| *setStringSetting* | `void setStringSetting(const char * key, const char * value)` |  |
| *App* | ` App()` |  |
| *main* | `void main()` | Your implementation of this must instantiate a ViewController and set it as _window.rootViewController before returning |
| *loadAsset* | `class `[`ByteBuffer`](/ref/data_group/ByteBuffer)` * loadAsset(const char * assetPath)` | Since this does IO it's best to limit use to app startup and background threads |
| *postToMainThread* | [`Task`](/ref/base_group/Task)` * postToMainThread(std::function< void(void)> func, int delay)` |  |
