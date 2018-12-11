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
| | |
|-|-|
|[`Canvas`](/ref/graphics_group/Canvas)` * create()`||


| | |
|-|-|
|`void resize(int width, int height)`||
|[`Bitmap`](/ref/graphics_group/Bitmap)` * getBitmap()`||
|`void clear(`[`COLOR`](/ref/graphics_group/COLOR)` color)`||
|`void setFillColor(`[`COLOR`](/ref/graphics_group/COLOR)` color)`||
|`void setStrokeColor(`[`COLOR`](/ref/graphics_group/COLOR)` color)`||
|`void setStrokeWidth(float strokeWidth)`||
|`void setAffineTransform(AFFINE_TRANSFORM * t)`||
|`void drawRect(RECT rect)`||
|`void drawOval(RECT rect)`||
|`void drawPath(Path * path)`||
|`void drawBitmap(`[`Bitmap`](/ref/graphics_group/Bitmap)` * bitmap, const RECT & rectSrc, const RECT & rectDst)`||
|`Path * createPath()`||


| | |
|-|-|
|` Canvas()`||


## Methods

| | |
|-|-|
| *create* | [`Canvas`](/ref/graphics_group/Canvas)` * create()` |  |
| *resize* | `void resize(int width, int height)` |  |
| *getBitmap* | [`Bitmap`](/ref/graphics_group/Bitmap)` * getBitmap()` |  |
| *clear* | `void clear(`[`COLOR`](/ref/graphics_group/COLOR)` color)` |  |
| *setFillColor* | `void setFillColor(`[`COLOR`](/ref/graphics_group/COLOR)` color)` |  |
| *setStrokeColor* | `void setStrokeColor(`[`COLOR`](/ref/graphics_group/COLOR)` color)` |  |
| *setStrokeWidth* | `void setStrokeWidth(float strokeWidth)` |  |
| *setAffineTransform* | `void setAffineTransform(AFFINE_TRANSFORM * t)` |  |
| *drawRect* | `void drawRect(RECT rect)` |  |
| *drawOval* | `void drawOval(RECT rect)` |  |
| *drawPath* | `void drawPath(Path * path)` |  |
| *drawBitmap* | `void drawBitmap(`[`Bitmap`](/ref/graphics_group/Bitmap)` * bitmap, const RECT & rectSrc, const RECT & rectDst)` |  |
| *createPath* | `Path * createPath()` |  |
| *Canvas* | ` Canvas()` |  |
