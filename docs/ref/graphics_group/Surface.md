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

` Surface(`[`View`](/oaknut/ref/views/View)` * owningView)`<br>

` ~Surface()`<br>

`void render(`[`View`](/oaknut/ref/views/View)` * view, `[`Window`](/oaknut/ref/views/Window)` * window)`<br>

`void setSize(SIZE size)`<br>

`void setupPrivateFbo()`<br>

`void use()`<br>

`void cleanup()`<br>

`void detachRenderList(RenderList * list)`<br>

`void attachRenderList(RenderList * list)`<br>

`void addRenderOp(RenderOp * op)`<br>

`void removeRenderOp(RenderOp * op)`<br>

`void validateRenderOps()`<br>

`void batchRenderOp(RenderOp * op)`<br>

`void unbatchRenderOp(RenderOp * op)`<br>



`void renderPhase1(`[`View`](/oaknut/ref/views/View)` * view, `[`Window`](/oaknut/ref/views/Window)` * window, POINT origin)`<br>

`void renderPhase2(`[`Surface`](/oaknut/ref/graphics_group/Surface)` * prevsurf, `[`View`](/oaknut/ref/views/View)` * view, `[`Window`](/oaknut/ref/views/Window)` * window)`<br>



## Methods

| | |
|-|-|
| *Surface* | ` Surface()` |  |
| *Surface* | ` Surface(`[`View`](/oaknut/ref/views/View)` * owningView)` |  |
| *addRenderOp* | `void addRenderOp(RenderOp * op)` |  |
| *attachRenderList* | `void attachRenderList(RenderList * list)` |  |
| *batchRenderOp* | `void batchRenderOp(RenderOp * op)` |  |
| *cleanup* | `void cleanup()` |  |
| *detachRenderList* | `void detachRenderList(RenderList * list)` |  |
| *removeRenderOp* | `void removeRenderOp(RenderOp * op)` |  |
| *render* | `void render(`[`View`](/oaknut/ref/views/View)` * view, `[`Window`](/oaknut/ref/views/Window)` * window)` |  |
| *renderPhase1* | `void renderPhase1(`[`View`](/oaknut/ref/views/View)` * view, `[`Window`](/oaknut/ref/views/Window)` * window, POINT origin)` |  |
| *renderPhase2* | `void renderPhase2(`[`Surface`](/oaknut/ref/graphics_group/Surface)` * prevsurf, `[`View`](/oaknut/ref/views/View)` * view, `[`Window`](/oaknut/ref/views/Window)` * window)` |  |
| *setSize* | `void setSize(SIZE size)` |  |
| *setupPrivateFbo* | `void setupPrivateFbo()` |  |
| *unbatchRenderOp* | `void unbatchRenderOp(RenderOp * op)` |  |
| *use* | `void use()` |  |
| *validateRenderOps* | `void validateRenderOps()` |  |
| *~Surface* | ` ~Surface()` |  |
