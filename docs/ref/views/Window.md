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
| | |
|-|-|
|` Window()`||


| | |
|-|-|
|[`Window`](/ref/views/Window)` * create()`||


| | |
|-|-|
|`void startAnimation(Animation * animation, int duration)`||
|`void startAnimation(Animation * animation, int duration, int delay)`||
|`void stopAnimation(Animation * animation)`||
|`void detachView(`[`View`](/ref/views/View)` * view)`||
|`bool hasPermission(Permission permission)`||
|`void runWithPermission(Permission permission, std::function< void(bool)> callback)`||
|`void runWithPermissions(vector< Permission > permission, std::function< void(vector< bool >)> callback)`||
|`void presentModalViewController(`[`ViewController`](/ref/app_group/ViewController)` * viewController)`||
|`void dismissModalViewController(`[`ViewController`](/ref/app_group/ViewController)` * viewController, std::function< void()> onComplete)`||
|`void setRootViewController(`[`ViewController`](/ref/app_group/ViewController)` * viewController)`||
|`void resizeSurface(int width, int height, float scale)`||
|`void destroySurface()`||
|`void draw()`||
|`void requestRedraw()`||
|`void requestRedrawNative()`||
|`void show()`||
|`void keyboardShow(bool show)`|Show or hide the system soft keyboard, if there is one.|
|`void keyboardNotifyTextChanged()`||
|`void keyboardNotifyTextSelectionChanged()`||
|`void dispatchInputEvent(INPUTEVENT event)`||
|`POINT offsetToView(`[`View`](/ref/views/View)` * view)`||
|`bool setFocusedView(`[`View`](/ref/views/View)` * view)`||
|`void attachViewController(`[`ViewController`](/ref/app_group/ViewController)` * vc)`||
|`void detachViewController(`[`ViewController`](/ref/app_group/ViewController)` * vc)`||
|`void pushClip(RECT clip)`||
|`void popClip()`||
|`void setBlendMode(int blendMode)`||
|`void bindTexture(`[`Bitmap`](/ref/graphics_group/Bitmap)` * texture)`||
|`void prepareToDraw()`||
|`void setCurrentSurface(`[`Surface`](/ref/graphics_group/Surface)` * surface)`||
|`void setVertexConfig(int vertexConfig)`||
|`void layout(RECT constraint)`|Recalculate and apply this view's size and position (as well as all subviews).|
|`void setSafeInsets(SafeInsetsType type, const EDGEINSETS & insets)`||
|`void ensureFocusedViewIsInSafeArea()`||
|`void updateDecorOp(bool bottom, float height)`||


## Methods

| | |
|-|-|
| *Window* | ` Window()` |  |
| *create* | [`Window`](/ref/views/Window)` * create()` |  |
| *startAnimation* | `void startAnimation(Animation * animation, int duration)` |  |
| *startAnimation* | `void startAnimation(Animation * animation, int duration, int delay)` |  |
| *stopAnimation* | `void stopAnimation(Animation * animation)` |  |
| *detachView* | `void detachView(`[`View`](/ref/views/View)` * view)` |  |
| *hasPermission* | `bool hasPermission(Permission permission)` |  |
| *runWithPermission* | `void runWithPermission(Permission permission, std::function< void(bool)> callback)` |  |
| *runWithPermissions* | `void runWithPermissions(vector< Permission > permission, std::function< void(vector< bool >)> callback)` |  |
| *presentModalViewController* | `void presentModalViewController(`[`ViewController`](/ref/app_group/ViewController)` * viewController)` |  |
| *dismissModalViewController* | `void dismissModalViewController(`[`ViewController`](/ref/app_group/ViewController)` * viewController, std::function< void()> onComplete)` |  |
| *setRootViewController* | `void setRootViewController(`[`ViewController`](/ref/app_group/ViewController)` * viewController)` |  |
| *resizeSurface* | `void resizeSurface(int width, int height, float scale)` |  |
| *destroySurface* | `void destroySurface()` |  |
| *draw* | `void draw()` |  |
| *requestRedraw* | `void requestRedraw()` |  |
| *requestRedrawNative* | `void requestRedrawNative()` |  |
| *show* | `void show()` |  |
| *keyboardShow* | `void keyboardShow(bool show)` |  |
| *keyboardNotifyTextChanged* | `void keyboardNotifyTextChanged()` |  |
| *keyboardNotifyTextSelectionChanged* | `void keyboardNotifyTextSelectionChanged()` |  |
| *dispatchInputEvent* | `void dispatchInputEvent(INPUTEVENT event)` |  |
| *offsetToView* | `POINT offsetToView(`[`View`](/ref/views/View)` * view)` |  |
| *setFocusedView* | `bool setFocusedView(`[`View`](/ref/views/View)` * view)` |  |
| *attachViewController* | `void attachViewController(`[`ViewController`](/ref/app_group/ViewController)` * vc)` |  |
| *detachViewController* | `void detachViewController(`[`ViewController`](/ref/app_group/ViewController)` * vc)` |  |
| *pushClip* | `void pushClip(RECT clip)` |  |
| *popClip* | `void popClip()` |  |
| *setBlendMode* | `void setBlendMode(int blendMode)` |  |
| *bindTexture* | `void bindTexture(`[`Bitmap`](/ref/graphics_group/Bitmap)` * texture)` |  |
| *prepareToDraw* | `void prepareToDraw()` |  |
| *setCurrentSurface* | `void setCurrentSurface(`[`Surface`](/ref/graphics_group/Surface)` * surface)` |  |
| *setVertexConfig* | `void setVertexConfig(int vertexConfig)` |  |
| *layout* | `void layout(RECT constraint)` | Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour. |
| *setSafeInsets* | `void setSafeInsets(SafeInsetsType type, const EDGEINSETS & insets)` |  |
| *ensureFocusedViewIsInSafeArea* | `void ensureFocusedViewIsInSafeArea()` |  |
| *updateDecorOp* | `void updateDecorOp(bool bottom, float height)` |  |
