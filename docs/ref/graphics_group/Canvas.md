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
## 

| `<a href="class_canvas.html">Canvas</a> * create()` |  |


## 

| `void resize(int width, int height)` |  |
| `<a href="class_bitmap.html">Bitmap</a> * getBitmap()` |  |
| `void clear(<a href="class_c_o_l_o_r.html">COLOR</a> color)` |  |
| `void setFillColor(<a href="class_c_o_l_o_r.html">COLOR</a> color)` |  |
| `void setStrokeColor(<a href="class_c_o_l_o_r.html">COLOR</a> color)` |  |
| `void setStrokeWidth(float strokeWidth)` |  |
| `void setAffineTransform(AFFINE_TRANSFORM * t)` |  |
| `void drawRect(RECT rect)` |  |
| `void drawOval(RECT rect)` |  |
| `void drawPath(Path * path)` |  |
| `void drawBitmap(<a href="class_bitmap.html">Bitmap</a> * bitmap, const RECT & rectSrc, const RECT & rectDst)` |  |
| `Path * createPath()` |  |


## 

| ` Canvas()` |  |


# Methods

| *create* |  `<a href="class_canvas.html">Canvas</a> * <a href="todo">create</a>()` |  |
| *resize* |  `void <a href="todo">resize</a>(int width, int height)` |  |
| *getBitmap* |  `<a href="class_bitmap.html">Bitmap</a> * <a href="todo">getBitmap</a>()` |  |
| *clear* |  `void <a href="todo">clear</a>(<a href="class_c_o_l_o_r.html">COLOR</a> color)` |  |
| *setFillColor* |  `void <a href="todo">setFillColor</a>(<a href="class_c_o_l_o_r.html">COLOR</a> color)` |  |
| *setStrokeColor* |  `void <a href="todo">setStrokeColor</a>(<a href="class_c_o_l_o_r.html">COLOR</a> color)` |  |
| *setStrokeWidth* |  `void <a href="todo">setStrokeWidth</a>(float strokeWidth)` |  |
| *setAffineTransform* |  `void <a href="todo">setAffineTransform</a>(AFFINE_TRANSFORM * t)` |  |
| *drawRect* |  `void <a href="todo">drawRect</a>(RECT rect)` |  |
| *drawOval* |  `void <a href="todo">drawOval</a>(RECT rect)` |  |
| *drawPath* |  `void <a href="todo">drawPath</a>(Path * path)` |  |
| *drawBitmap* |  `void <a href="todo">drawBitmap</a>(<a href="class_bitmap.html">Bitmap</a> * bitmap, const RECT & rectSrc, const RECT & rectDst)` |  |
| *createPath* |  `Path * <a href="todo">createPath</a>()` |  |
| *Canvas* |  ` <a href="todo">Canvas</a>()` |  |
