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
## 



## 



## 

| ` Surface()` |  |
| ` Surface(<a href="class_view.html">View</a> * owningView)` |  |
| ` ~Surface()` |  |
| `void render(<a href="class_view.html">View</a> * view, <a href="class_window.html">Window</a> * window)` |  |
| `void setSize(SIZE size)` |  |
| `void setupPrivateFbo()` |  |
| `void use()` |  |
| `void cleanup()` |  |
| `void detachRenderList(RenderList * list)` |  |
| `void attachRenderList(RenderList * list)` |  |
| `void addRenderOp(RenderOp * op)` |  |
| `void removeRenderOp(RenderOp * op)` |  |
| `void validateRenderOps()` |  |
| `void batchRenderOp(RenderOp * op)` |  |
| `void unbatchRenderOp(RenderOp * op)` |  |


## 

| `void renderPhase1(<a href="class_view.html">View</a> * view, <a href="class_window.html">Window</a> * window, POINT origin)` |  |
| `void renderPhase2(<a href="class_surface.html">Surface</a> * prevsurf, <a href="class_view.html">View</a> * view, <a href="class_window.html">Window</a> * window)` |  |


# Methods

| *Surface* |  ` <a href="todo">Surface</a>()` |  |
| *Surface* |  ` <a href="todo">Surface</a>(<a href="class_view.html">View</a> * owningView)` |  |
| *~Surface* |  ` <a href="todo">~Surface</a>()` |  |
| *render* |  `void <a href="todo">render</a>(<a href="class_view.html">View</a> * view, <a href="class_window.html">Window</a> * window)` |  |
| *setSize* |  `void <a href="todo">setSize</a>(SIZE size)` |  |
| *setupPrivateFbo* |  `void <a href="todo">setupPrivateFbo</a>()` |  |
| *use* |  `void <a href="todo">use</a>()` |  |
| *cleanup* |  `void <a href="todo">cleanup</a>()` |  |
| *detachRenderList* |  `void <a href="todo">detachRenderList</a>(RenderList * list)` |  |
| *attachRenderList* |  `void <a href="todo">attachRenderList</a>(RenderList * list)` |  |
| *addRenderOp* |  `void <a href="todo">addRenderOp</a>(RenderOp * op)` |  |
| *removeRenderOp* |  `void <a href="todo">removeRenderOp</a>(RenderOp * op)` |  |
| *validateRenderOps* |  `void <a href="todo">validateRenderOps</a>()` |  |
| *batchRenderOp* |  `void <a href="todo">batchRenderOp</a>(RenderOp * op)` |  |
| *unbatchRenderOp* |  `void <a href="todo">unbatchRenderOp</a>(RenderOp * op)` |  |
| *renderPhase1* |  `void <a href="todo">renderPhase1</a>(<a href="class_view.html">View</a> * view, <a href="class_window.html">Window</a> * window, POINT origin)` |  |
| *renderPhase2* |  `void <a href="todo">renderPhase2</a>(<a href="class_surface.html">Surface</a> * prevsurf, <a href="class_view.html">View</a> * view, <a href="class_window.html">Window</a> * window)` |  |
