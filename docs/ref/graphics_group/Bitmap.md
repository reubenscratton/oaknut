---
layout: default
---

# Bitmap

```
class Bitmap
    : public Object, 
public ISerializeToVariant
```


The standard 2D array of color values.     

Bitmap is a platform type, i.e. apps must instantiate them via one of the Bitmap::createXXX() APIs, never with operator new().The Bitmap class does not directly own a block of RAM for pixel data, the storage of pixel data is left to the platform layer. When application or Oaknut code needs to access the pixel data it has to lock() and unlock() the bitmap. This may or may not cause an expensive blit, it's up to the platform. Only access pixel data when you really need to.Bitmap formats ============== Oaknut bitmaps have three core formats: RGBA32, RGB16, and A8. Apple platforms allows a fourth format, BGRA32, in order to avoid needless conversion.On the web the only format supported is RGBA32 so bitmap data in other formats will be converted.     
## 



## 

| ` Bitmap(int width, int height, int format)` |  |
| ` Bitmap()` |  |
| ` ~Bitmap()` |  |


## 

| `void lock(PIXELDATA * pixelData, bool forWriting)` |  |
| `void unlock(PIXELDATA * pixelData, bool pixelsChanged)` |  |
| `uint8_t * pixelAddress(PIXELDATA * pixelData, int x, int y)` |  |
| `void bind()` |  |
| `void onRenderContextDestroyed()` |  |
| `int getBytesPerPixel()` |  |
| `GLenum getGlFormat()` |  |
| `GLenum getGlInternalFormat()` |  |
| `int getGlPixelType()` |  |
| `bool hasAlpha()` |  |
| `bool hasPremultipliedAlpha()` |  |
| `int sizeInBytes()` |  |
| `<a href="class_bitmap.html">Bitmap</a> * convertToFormat(int newFormat)` |  |
| `void fromVariant(const <a href="classvariant.html">variant</a> & v)` |  |
| `void toVariant(<a href="classvariant.html">variant</a> & v)` |  |


## 

| `<a href="class_bitmap.html">Bitmap</a> * create(int width, int height, int format)` |  |
| `<a href="class_task.html">Task</a> * createFromData(const void * data, int cb, std::function< void(<a href="class_bitmap.html">Bitmap</a> *)> callback)` |  |


# Methods

| *Bitmap* |  ` <a href="todo">Bitmap</a>(int width, int height, int format)` |  |
| *Bitmap* |  ` <a href="todo">Bitmap</a>()` |  |
| *~Bitmap* |  ` <a href="todo">~Bitmap</a>()` |  |
| *lock* |  `void <a href="todo">lock</a>(PIXELDATA * pixelData, bool forWriting)` |  |
| *unlock* |  `void <a href="todo">unlock</a>(PIXELDATA * pixelData, bool pixelsChanged)` |  |
| *pixelAddress* |  `uint8_t * <a href="todo">pixelAddress</a>(PIXELDATA * pixelData, int x, int y)` |  |
| *bind* |  `void <a href="todo">bind</a>()` |  |
| *onRenderContextDestroyed* |  `void <a href="todo">onRenderContextDestroyed</a>()` |  |
| *getBytesPerPixel* |  `int <a href="todo">getBytesPerPixel</a>()` |  |
| *getGlFormat* |  `GLenum <a href="todo">getGlFormat</a>()` |  |
| *getGlInternalFormat* |  `GLenum <a href="todo">getGlInternalFormat</a>()` |  |
| *getGlPixelType* |  `int <a href="todo">getGlPixelType</a>()` |  |
| *hasAlpha* |  `bool <a href="todo">hasAlpha</a>()` |  |
| *hasPremultipliedAlpha* |  `bool <a href="todo">hasPremultipliedAlpha</a>()` |  |
| *sizeInBytes* |  `int <a href="todo">sizeInBytes</a>()` |  |
| *convertToFormat* |  `<a href="class_bitmap.html">Bitmap</a> * <a href="todo">convertToFormat</a>(int newFormat)` |  |
| *fromVariant* |  `void <a href="todo">fromVariant</a>(const <a href="classvariant.html">variant</a> & v)` |  |
| *toVariant* |  `void <a href="todo">toVariant</a>(<a href="classvariant.html">variant</a> & v)` |  |
| *create* |  `<a href="class_bitmap.html">Bitmap</a> * <a href="todo">create</a>(int width, int height, int format)` |  |
| *createFromData* |  `<a href="class_task.html">Task</a> * <a href="todo">createFromData</a>(const void * data, int cb, std::function< void(<a href="class_bitmap.html">Bitmap</a> *)> callback)` |  |
