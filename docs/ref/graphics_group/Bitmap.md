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
| | |
|-|-|
|` Bitmap(int width, int height, int format)`||
|` Bitmap()`||
|` ~Bitmap()`||


| | |
|-|-|
|`void lock(PIXELDATA * pixelData, bool forWriting)`||
|`void unlock(PIXELDATA * pixelData, bool pixelsChanged)`||
|`uint8_t * pixelAddress(PIXELDATA * pixelData, int x, int y)`||
|`void bind()`||
|`void onRenderContextDestroyed()`||
|`int getBytesPerPixel()`||
|`GLenum getGlFormat()`||
|`GLenum getGlInternalFormat()`||
|`int getGlPixelType()`||
|`bool hasAlpha()`||
|`bool hasPremultipliedAlpha()`||
|`int sizeInBytes()`||
|[`Bitmap`](/ref/graphics_group/Bitmap)` * convertToFormat(int newFormat)`||
|`void fromVariant(const variant & v)`||
|`void toVariant(variant & v)`||


| | |
|-|-|
|[`Bitmap`](/ref/graphics_group/Bitmap)` * create(int width, int height, int format)`||
|[`Task`](/ref/base_group/Task)` * createFromData(const void * data, int cb, std::function< void(`[`Bitmap`](/ref/graphics_group/Bitmap)` *)> callback)`||


## Methods

| | |
|-|-|
| *Bitmap* | ` Bitmap(int width, int height, int format)` |  |
| *Bitmap* | ` Bitmap()` |  |
| *~Bitmap* | ` ~Bitmap()` |  |
| *lock* | `void lock(PIXELDATA * pixelData, bool forWriting)` |  |
| *unlock* | `void unlock(PIXELDATA * pixelData, bool pixelsChanged)` |  |
| *pixelAddress* | `uint8_t * pixelAddress(PIXELDATA * pixelData, int x, int y)` |  |
| *bind* | `void bind()` |  |
| *onRenderContextDestroyed* | `void onRenderContextDestroyed()` |  |
| *getBytesPerPixel* | `int getBytesPerPixel()` |  |
| *getGlFormat* | `GLenum getGlFormat()` |  |
| *getGlInternalFormat* | `GLenum getGlInternalFormat()` |  |
| *getGlPixelType* | `int getGlPixelType()` |  |
| *hasAlpha* | `bool hasAlpha()` |  |
| *hasPremultipliedAlpha* | `bool hasPremultipliedAlpha()` |  |
| *sizeInBytes* | `int sizeInBytes()` |  |
| *convertToFormat* | [`Bitmap`](/ref/graphics_group/Bitmap)` * convertToFormat(int newFormat)` |  |
| *fromVariant* | `void fromVariant(const variant & v)` |  |
| *toVariant* | `void toVariant(variant & v)` |  |
| *create* | [`Bitmap`](/ref/graphics_group/Bitmap)` * create(int width, int height, int format)` |  |
| *createFromData* | [`Task`](/ref/base_group/Task)` * createFromData(const void * data, int cb, std::function< void(`[`Bitmap`](/ref/graphics_group/Bitmap)` *)> callback)` |  |
