---
layout: default
---

# Canvas

```
class Canvas
    : public Object
```


Basic 2D Canvas API.     

Canvas is a platform type, instantiate via `Canvas::create()`.NB: This is VERY limited at present, features get added as they become needed.     
[`Canvas`](/oaknut/ref/graphics_group/Canvas)`* create()`<br>

`void resize(int width, int height)`<br>
[`Bitmap`](/oaknut/ref/graphics_group/Bitmap)`* getBitmap()`<br>
`void clear(`[`COLOR`](/oaknut/ref/graphics_group/COLOR)` color)`<br>
`void setFillColor(`[`COLOR`](/oaknut/ref/graphics_group/COLOR)` color)`<br>
`void setStrokeColor(`[`COLOR`](/oaknut/ref/graphics_group/COLOR)` color)`<br>
`void setStrokeWidth(float strokeWidth)`<br>
`void setAffineTransform(AFFINE_TRANSFORM* t)`<br>
`void drawRect(RECT rect)`<br>
`void drawOval(RECT rect)`<br>
`void drawPath(Path* path)`<br>
`void drawBitmap(`[`Bitmap`](/oaknut/ref/graphics_group/Bitmap)`* bitmap, const RECT & rectSrc, const RECT & rectDst)`<br>
`Path* createPath()`<br>

` Canvas()`<br>

