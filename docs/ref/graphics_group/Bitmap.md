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
` Bitmap(int width, int height, int format)`<br>

` Bitmap()`<br>

` ~Bitmap()`<br>


`void lock(PIXELDATA* pixelData, bool forWriting)`<br>

`void unlock(PIXELDATA* pixelData, bool pixelsChanged)`<br>

`uint8_t* pixelAddress(PIXELDATA* pixelData, int x, int y)`<br>

`void bind()`<br>

`void onRenderContextDestroyed()`<br>

`int getBytesPerPixel()`<br>

`GLenum getGlFormat()`<br>

`GLenum getGlInternalFormat()`<br>

`int getGlPixelType()`<br>

`bool hasAlpha()`<br>

`bool hasPremultipliedAlpha()`<br>

`int sizeInBytes()`<br>

[`Bitmap`](/ref/graphics_group/Bitmap)`* convertToFormat(int newFormat)`<br>

`void fromVariant(const variant & v)`<br>

`void toVariant(variant & v)`<br>


[`Bitmap`](/ref/graphics_group/Bitmap)`* create(int width, int height, int format)`<br>

[`Task`](/ref/base_group/Task)`* createFromData(const void* data, int cb, std::function< void(`[`Bitmap`](/ref/graphics_group/Bitmap)`*)> callback)`<br>


