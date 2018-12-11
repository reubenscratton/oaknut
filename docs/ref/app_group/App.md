---
layout: default
---

# App

```
class App
    : public Object
```


The single instance of this class is accessed through the global 'app' variable.     

    
## Time

| `TIMESTAMP currentMillis()` | Gets the current system time, in milliseconds. |
| `<a href="classstring.html">string</a> friendlyTimeString(TIMESTAMP timestamp)` |  |


## Logging

| `void log(char const * fmt, ... )` | Log an informational message. |
| `void warn(char const * fmt, ... )` | Log a warning message prefixed with "Warning: ". |


## File paths

| `<a href="classstring.html">string</a> getPathForGeneralFiles()` |  |
| `<a href="classstring.html">string</a> getPathForUserDocuments()` |  |
| `<a href="classstring.html">string</a> getPathForCacheFiles()` |  |
| `<a href="classstring.html">string</a> getPathForTemporaryFiles()` |  |


## Styles

| `void loadStyleAsset(const <a href="classstring.html">string</a> & assetPath)` | Load a style asset from the assets directory. |
| `const <a href="class_style_value.html">StyleValue</a> * getStyleValue(const <a href="classstring.html">string</a> & keypath)` | Get a named style value. |
| `<a href="classstring.html">string</a> getStyleString(const <a href="classstring.html">string</a> & key, const char * defaultString)` | Get a named style value and coerce to a string. |
| `float getStyleFloat(const <a href="classstring.html">string</a> & key)` | Get a named style value and coerce to a float. |
| `<a href="class_c_o_l_o_r.html">COLOR</a> getStyleColor(const <a href="classstring.html">string</a> & key)` | Get a named style value and coerce to a COLOR. |


## Layout

| `class <a href="class_view.html">View</a> * layoutInflate(const <a href="classstring.html">string</a> & assetPath)` | Loads a view from an asset file. |
| `void layoutInflateExistingView(<a href="class_view.html">View</a> * view, const <a href="classstring.html">string</a> & assetPath)` |  |
| `float dp(float dp)` | Converts a 'dp' measurement into physical pixels. |
| `float idp(float pix)` | Converts a 'dp' measurement into physical pixels and floor()s the result to an integer. |


## Locale

| `<a href="classstring.html">string</a> currentCountryCode()` | Returns ISO-3166 two-letter country code the device is configured for, which is not necessarily the country the device is currently in. |


## Settings

| `int getIntSetting(const char * key, const int defaultValue)` | Gets a named integer setting. |
| `void setIntSetting(const char * key, int value)` | Sets a named integer setting. |
| `<a href="classstring.html">string</a> getStringSetting(const char * key, const char * defaultValue)` | Gets a named string setting. |
| `void setStringSetting(const char * key, const char * value)` | Sets a named string setting. |


## 



## 



## 

| ` App()` |  |
| `void main()` | Application entry point. |
| `class <a href="class_byte_buffer.html">ByteBuffer</a> * loadAsset(const char * assetPath)` | Load a file from the assets directory, synchronously. |


## 

| `<a href="class_task.html">Task</a> * postToMainThread(std::function< void(void)> func, int delay)` |  |


# Methods

| *currentMillis* |  `TIMESTAMP <a href="todo">currentMillis</a>()` |  |
| *friendlyTimeString* |  `<a href="classstring.html">string</a> <a href="todo">friendlyTimeString</a>(TIMESTAMP timestamp)` |  |
| *log* |  `void <a href="todo">log</a>(char const * fmt, ... )` |  |
| *warn* |  `void <a href="todo">warn</a>(char const * fmt, ... )` |  |
| *getPathForGeneralFiles* |  `<a href="classstring.html">string</a> <a href="todo">getPathForGeneralFiles</a>()` |  |
| *getPathForUserDocuments* |  `<a href="classstring.html">string</a> <a href="todo">getPathForUserDocuments</a>()` |  |
| *getPathForCacheFiles* |  `<a href="classstring.html">string</a> <a href="todo">getPathForCacheFiles</a>()` |  |
| *getPathForTemporaryFiles* |  `<a href="classstring.html">string</a> <a href="todo">getPathForTemporaryFiles</a>()` |  |
| *loadStyleAsset* |  `void <a href="todo">loadStyleAsset</a>(const <a href="classstring.html">string</a> & assetPath)` | Generally used in App::main() |
| *getStyleValue* |  `const <a href="class_style_value.html">StyleValue</a> * <a href="todo">getStyleValue</a>(const <a href="classstring.html">string</a> & keypath)` |  |
| *getStyleString* |  `<a href="classstring.html">string</a> <a href="todo">getStyleString</a>(const <a href="classstring.html">string</a> & key, const char * defaultString)` |  |
| *getStyleFloat* |  `float <a href="todo">getStyleFloat</a>(const <a href="classstring.html">string</a> & key)` |  |
| *getStyleColor* |  `<a href="class_c_o_l_o_r.html">COLOR</a> <a href="todo">getStyleColor</a>(const <a href="classstring.html">string</a> & key)` |  |
| *layoutInflate* |  `class <a href="class_view.html">View</a> * <a href="todo">layoutInflate</a>(const <a href="classstring.html">string</a> & assetPath)` | See the Layout Guide for details |
| *layoutInflateExistingView* |  `void <a href="todo">layoutInflateExistingView</a>(<a href="class_view.html">View</a> * view, const <a href="classstring.html">string</a> & assetPath)` |  |
| *dp* |  `float <a href="todo">dp</a>(float dp)` | Dp is a device-independent pixel and is based on 320dpi |
| *idp* |  `float <a href="todo">idp</a>(float pix)` |  |
| *currentCountryCode* |  `<a href="classstring.html">string</a> <a href="todo">currentCountryCode</a>()` |  |
| *getIntSetting* |  `int <a href="todo">getIntSetting</a>(const char * key, const int defaultValue)` |  |
| *setIntSetting* |  `void <a href="todo">setIntSetting</a>(const char * key, int value)` |  |
| *getStringSetting* |  `<a href="classstring.html">string</a> <a href="todo">getStringSetting</a>(const char * key, const char * defaultValue)` |  |
| *setStringSetting* |  `void <a href="todo">setStringSetting</a>(const char * key, const char * value)` |  |
| *App* |  ` <a href="todo">App</a>()` |  |
| *main* |  `void <a href="todo">main</a>()` | Your implementation of this must instantiate a ViewController and set it as _window.rootViewController before returning |
| *loadAsset* |  `class <a href="class_byte_buffer.html">ByteBuffer</a> * <a href="todo">loadAsset</a>(const char * assetPath)` | Since this does IO it's best to limit use to app startup and background threads |
| *postToMainThread* |  `<a href="class_task.html">Task</a> * <a href="todo">postToMainThread</a>(std::function< void(void)> func, int delay)` |  |
