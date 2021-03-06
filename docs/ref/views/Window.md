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
` Window()`<br>


[`Window`](/ref/views/Window)`* create()`<br>


`void startAnimation(`[`Animation`](/ref/views/Animation)`* animation, int duration)`<br>

`void startAnimation(`[`Animation`](/ref/views/Animation)`* animation, int duration, int delay)`<br>

`void stopAnimation(`[`Animation`](/ref/views/Animation)`* animation)`<br>

`void detachView(`[`View`](/ref/views/View)`* view)`<br>

`bool hasPermission(Permission permission)`<br>

`void runWithPermission(Permission permission, std::function< void(bool)> callback)`<br>

`void runWithPermissions(vector< Permission > permission, std::function< void(vector< bool >)> callback)`<br>

`void presentModalViewController(`[`ViewController`](/ref/app_group/ViewController)`* viewController)`<br>

`void dismissModalViewController(`[`ViewController`](/ref/app_group/ViewController)`* viewController, std::function< void()> onComplete)`<br>

`void setRootViewController(`[`ViewController`](/ref/app_group/ViewController)`* viewController)`<br>

`void resizeSurface(int width, int height, float scale)`<br>

`void destroySurface()`<br>

`void draw()`<br>

`void requestRedraw()`<br>

`void requestRedrawNative()`<br>

`void show()`<br>

`void keyboardShow(bool show)`<br>Show or hide the system soft keyboard, if there is one.

`void keyboardNotifyTextChanged()`<br>

`void keyboardNotifyTextSelectionChanged()`<br>

`void dispatchInputEvent(INPUTEVENT event)`<br>

`POINT offsetToView(`[`View`](/ref/views/View)`* view)`<br>

`bool setFocusedView(`[`View`](/ref/views/View)`* view)`<br>

`void attachViewController(`[`ViewController`](/ref/app_group/ViewController)`* vc)`<br>

`void detachViewController(`[`ViewController`](/ref/app_group/ViewController)`* vc)`<br>

`void pushClip(RECT clip)`<br>

`void popClip()`<br>

`void setBlendMode(int blendMode)`<br>

`void bindTexture(`[`Bitmap`](/ref/graphics_group/Bitmap)`* texture)`<br>

`void prepareToDraw()`<br>

`void setCurrentSurface(`[`Surface`](/ref/graphics_group/Surface)`* surface)`<br>

`void setVertexConfig(int vertexConfig)`<br>

`void layout(RECT constraint)`<br>Recalculate and apply this view's size and position (as well as all subviews).
Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour.

`void setSafeInsets(SafeInsetsType type, const EDGEINSETS & insets)`<br>

`void ensureFocusedViewIsInSafeArea()`<br>

`void updateDecorOp(bool bottom, float height)`<br>


