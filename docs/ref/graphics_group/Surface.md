---
layout: default
---

# Surface

```
class Surface
    : public Object
```


Rendering target, currently backed by a GL texture.     

Surfaces are owned by Views, all visible Views in the owning view's hierarchy will be rendered to the surface.     
` Surface()`<br>

` Surface(`[`View`](/ref/views/View)`* owningView)`<br>

` ~Surface()`<br>

`void render(`[`View`](/ref/views/View)`* view, `[`Window`](/ref/views/Window)`* window)`<br>

`void setSize(SIZE size)`<br>

`void setupPrivateFbo()`<br>

`void use()`<br>

`void cleanup()`<br>

`void detachRenderList(RenderList* list)`<br>

`void attachRenderList(RenderList* list)`<br>

`void addRenderOp(RenderOp* op)`<br>

`void removeRenderOp(RenderOp* op)`<br>

`void validateRenderOps()`<br>

`void batchRenderOp(RenderOp* op)`<br>

`void unbatchRenderOp(RenderOp* op)`<br>


`void renderPhase1(`[`View`](/ref/views/View)`* view, `[`Window`](/ref/views/Window)`* window, POINT origin)`<br>

`void renderPhase2(`[`Surface`](/ref/graphics_group/Surface)`* prevsurf, `[`View`](/ref/views/View)`* view, `[`Window`](/ref/views/Window)`* window)`<br>


