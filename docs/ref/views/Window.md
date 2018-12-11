---
layout: default
---

# Window

```
class Window
    : public View
```


A window is the top level container for app UI.     

There is usually only one Window instance, globally accessible through `app.window`. It usually wraps a native window in some way.     
## 



## 

| ` Window()` |  |


## 

| `<a href="class_window.html">Window</a> * create()` |  |


## 

| `void startAnimation(Animation * animation, int duration)` |  |
| `void startAnimation(Animation * animation, int duration, int delay)` |  |
| `void stopAnimation(Animation * animation)` |  |
| `void detachView(<a href="class_view.html">View</a> * view)` |  |
| `bool hasPermission(Permission permission)` |  |
| `void runWithPermission(Permission permission, std::function< void(bool)> callback)` |  |
| `void runWithPermissions(vector< Permission > permission, std::function< void(vector< bool >)> callback)` |  |
| `void presentModalViewController(<a href="class_view_controller.html">ViewController</a> * viewController)` |  |
| `void dismissModalViewController(<a href="class_view_controller.html">ViewController</a> * viewController, std::function< void()> onComplete)` |  |
| `void setRootViewController(<a href="class_view_controller.html">ViewController</a> * viewController)` |  |
| `void resizeSurface(int width, int height, float scale)` |  |
| `void destroySurface()` |  |
| `void draw()` |  |
| `void requestRedraw()` |  |
| `void requestRedrawNative()` |  |
| `void show()` |  |
| `void keyboardShow(bool show)` | Show or hide the system soft keyboard, if there is one. |
| `void keyboardNotifyTextChanged()` |  |
| `void keyboardNotifyTextSelectionChanged()` |  |
| `void dispatchInputEvent(INPUTEVENT event)` |  |
| `POINT offsetToView(<a href="class_view.html">View</a> * view)` |  |
| `bool setFocusedView(<a href="class_view.html">View</a> * view)` |  |
| `void attachViewController(<a href="class_view_controller.html">ViewController</a> * vc)` |  |
| `void detachViewController(<a href="class_view_controller.html">ViewController</a> * vc)` |  |
| `void pushClip(RECT clip)` |  |
| `void popClip()` |  |
| `void setBlendMode(int blendMode)` |  |
| `void bindTexture(<a href="class_bitmap.html">Bitmap</a> * texture)` |  |
| `void prepareToDraw()` |  |
| `void setCurrentSurface(<a href="class_surface.html">Surface</a> * surface)` |  |
| `void setVertexConfig(int vertexConfig)` |  |
| `void layout(RECT constraint)` | Recalculate and apply this view's size and position (as well as all subviews). |
| `void setSafeInsets(SafeInsetsType type, const EDGEINSETS & insets)` |  |
| `void ensureFocusedViewIsInSafeArea()` |  |
| `void updateDecorOp(bool bottom, float height)` |  |


# Methods

| *Window* |  ` <a href="todo">Window</a>()` |  |
| *create* |  `<a href="class_window.html">Window</a> * <a href="todo">create</a>()` |  |
| *startAnimation* |  `void <a href="todo">startAnimation</a>(Animation * animation, int duration)` |  |
| *startAnimation* |  `void <a href="todo">startAnimation</a>(Animation * animation, int duration, int delay)` |  |
| *stopAnimation* |  `void <a href="todo">stopAnimation</a>(Animation * animation)` |  |
| *detachView* |  `void <a href="todo">detachView</a>(<a href="class_view.html">View</a> * view)` |  |
| *hasPermission* |  `bool <a href="todo">hasPermission</a>(Permission permission)` |  |
| *runWithPermission* |  `void <a href="todo">runWithPermission</a>(Permission permission, std::function< void(bool)> callback)` |  |
| *runWithPermissions* |  `void <a href="todo">runWithPermissions</a>(vector< Permission > permission, std::function< void(vector< bool >)> callback)` |  |
| *presentModalViewController* |  `void <a href="todo">presentModalViewController</a>(<a href="class_view_controller.html">ViewController</a> * viewController)` |  |
| *dismissModalViewController* |  `void <a href="todo">dismissModalViewController</a>(<a href="class_view_controller.html">ViewController</a> * viewController, std::function< void()> onComplete)` |  |
| *setRootViewController* |  `void <a href="todo">setRootViewController</a>(<a href="class_view_controller.html">ViewController</a> * viewController)` |  |
| *resizeSurface* |  `void <a href="todo">resizeSurface</a>(int width, int height, float scale)` |  |
| *destroySurface* |  `void <a href="todo">destroySurface</a>()` |  |
| *draw* |  `void <a href="todo">draw</a>()` |  |
| *requestRedraw* |  `void <a href="todo">requestRedraw</a>()` |  |
| *requestRedrawNative* |  `void <a href="todo">requestRedrawNative</a>()` |  |
| *show* |  `void <a href="todo">show</a>()` |  |
| *keyboardShow* |  `void <a href="todo">keyboardShow</a>(bool show)` |  |
| *keyboardNotifyTextChanged* |  `void <a href="todo">keyboardNotifyTextChanged</a>()` |  |
| *keyboardNotifyTextSelectionChanged* |  `void <a href="todo">keyboardNotifyTextSelectionChanged</a>()` |  |
| *dispatchInputEvent* |  `void <a href="todo">dispatchInputEvent</a>(INPUTEVENT event)` |  |
| *offsetToView* |  `POINT <a href="todo">offsetToView</a>(<a href="class_view.html">View</a> * view)` |  |
| *setFocusedView* |  `bool <a href="todo">setFocusedView</a>(<a href="class_view.html">View</a> * view)` |  |
| *attachViewController* |  `void <a href="todo">attachViewController</a>(<a href="class_view_controller.html">ViewController</a> * vc)` |  |
| *detachViewController* |  `void <a href="todo">detachViewController</a>(<a href="class_view_controller.html">ViewController</a> * vc)` |  |
| *pushClip* |  `void <a href="todo">pushClip</a>(RECT clip)` |  |
| *popClip* |  `void <a href="todo">popClip</a>()` |  |
| *setBlendMode* |  `void <a href="todo">setBlendMode</a>(int blendMode)` |  |
| *bindTexture* |  `void <a href="todo">bindTexture</a>(<a href="class_bitmap.html">Bitmap</a> * texture)` |  |
| *prepareToDraw* |  `void <a href="todo">prepareToDraw</a>()` |  |
| *setCurrentSurface* |  `void <a href="todo">setCurrentSurface</a>(<a href="class_surface.html">Surface</a> * surface)` |  |
| *setVertexConfig* |  `void <a href="todo">setVertexConfig</a>(int vertexConfig)` |  |
| *layout* |  `void <a href="todo">layout</a>(RECT constraint)` | Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour. |
| *setSafeInsets* |  `void <a href="todo">setSafeInsets</a>(SafeInsetsType type, const EDGEINSETS & insets)` |  |
| *ensureFocusedViewIsInSafeArea* |  `void <a href="todo">ensureFocusedViewIsInSafeArea</a>()` |  |
| *updateDecorOp* |  `void <a href="todo">updateDecorOp</a>(bool bottom, float height)` |  |
