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
| | |
|-|-|
|` Surface()`||
|` Surface(`[`View`](/ref/views/View)` * owningView)`||
|` ~Surface()`||
|`void render(`[`View`](/ref/views/View)` * view, `[`Window`](/ref/views/Window)` * window)`||
|`void setSize(SIZE size)`||
|`void setupPrivateFbo()`||
|`void use()`||
|`void cleanup()`||
|`void detachRenderList(RenderList * list)`||
|`void attachRenderList(RenderList * list)`||
|`void addRenderOp(RenderOp * op)`||
|`void removeRenderOp(RenderOp * op)`||
|`void validateRenderOps()`||
|`void batchRenderOp(RenderOp * op)`||
|`void unbatchRenderOp(RenderOp * op)`||


| | |
|-|-|
|`void renderPhase1(`[`View`](/ref/views/View)` * view, `[`Window`](/ref/views/Window)` * window, POINT origin)`||
|`void renderPhase2(`[`Surface`](/ref/graphics_group/Surface)` * prevsurf, `[`View`](/ref/views/View)` * view, `[`Window`](/ref/views/Window)` * window)`||


## Methods

| | |
|-|-|
| *Surface* | ` Surface()` |  |
| *Surface* | ` Surface(`[`View`](/ref/views/View)` * owningView)` |  |
| *~Surface* | ` ~Surface()` |  |
| *render* | `void render(`[`View`](/ref/views/View)` * view, `[`Window`](/ref/views/Window)` * window)` |  |
| *setSize* | `void setSize(SIZE size)` |  |
| *setupPrivateFbo* | `void setupPrivateFbo()` |  |
| *use* | `void use()` |  |
| *cleanup* | `void cleanup()` |  |
| *detachRenderList* | `void detachRenderList(RenderList * list)` |  |
| *attachRenderList* | `void attachRenderList(RenderList * list)` |  |
| *addRenderOp* | `void addRenderOp(RenderOp * op)` |  |
| *removeRenderOp* | `void removeRenderOp(RenderOp * op)` |  |
| *validateRenderOps* | `void validateRenderOps()` |  |
| *batchRenderOp* | `void batchRenderOp(RenderOp * op)` |  |
| *unbatchRenderOp* | `void unbatchRenderOp(RenderOp * op)` |  |
| *renderPhase1* | `void renderPhase1(`[`View`](/ref/views/View)` * view, `[`Window`](/ref/views/Window)` * window, POINT origin)` |  |
| *renderPhase2* | `void renderPhase2(`[`Surface`](/ref/graphics_group/Surface)` * prevsurf, `[`View`](/ref/views/View)` * view, `[`Window`](/ref/views/Window)` * window)` |  |
