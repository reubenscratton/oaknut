---
layout: default
---

# ViewController

```
class ViewController
    : public Object
```


Owner of a set of views that collectively make up a discrete piece of user-facing behaviour.     

It is most often a complete screen but doesn't have to be.     
### Events

`void onWindowAttached()`<br>Called when this view controller is attached to the window.

`void onWindowDetached()`<br>

`void onWillAppear(bool firstTime)`<br>Navigation events.

`void onDidAppear(bool firstTime)`<br>

`void onWillDisappear(bool lastTime)`<br>

`void onDidDisappear(bool lastTime)`<br>

`void onBackButtonClicked()`<br>Called when the user navigates 'back'.



[`string`](/oaknut/ref/base_group/string)` getTitle()`<br>

`void setTitle(const `[`string`](/oaknut/ref/base_group/string)` & title)`<br>

`void setTitleView(`[`View`](/oaknut/ref/views/View)` * titleView)`<br>

`class ToolbarButton * addNavButton(bool rightSide, const `[`string`](/oaknut/ref/base_group/string)` & assetPath, std::function< void()> onClick)`<br>

` ViewController()`<br>

` ~ViewController()`<br>

[`View`](/oaknut/ref/views/View)` * getView()`<br>Get the root view.

`void setView(`[`View`](/oaknut/ref/views/View)` * view)`<br>Set the root view.

[`Window`](/oaknut/ref/views/Window)` * getWindow()`<br>Get the Window that the root view is attached to.

[`View`](/oaknut/ref/views/View)` * inflate(const `[`string`](/oaknut/ref/base_group/string)` & layoutAsset)`<br>

`void bind(T *& rview, const `[`string`](/oaknut/ref/base_group/string)` & id)`<br>

`void applySafeInsets(const EDGEINSETS & safeInsets)`<br>

`bool navigateBack()`<br>

`void requestScroll(float dx, float dy)`<br>



## Methods

| | |
|-|-|
| *ViewController* | ` ViewController()` |  |
| *addNavButton* | `class ToolbarButton * addNavButton(bool rightSide, const `[`string`](/oaknut/ref/base_group/string)` & assetPath, std::function< void()> onClick)` |  |
| *applySafeInsets* | `void applySafeInsets(const EDGEINSETS & safeInsets)` |  |
| *bind* | `void bind(T *& rview, const `[`string`](/oaknut/ref/base_group/string)` & id)` |  |
| *getTitle* | [`string`](/oaknut/ref/base_group/string)` getTitle()` |  |
| *getView* | [`View`](/oaknut/ref/views/View)` * getView()` |  |
| *getWindow* | [`Window`](/oaknut/ref/views/Window)` * getWindow()` |  |
| *inflate* | [`View`](/oaknut/ref/views/View)` * inflate(const `[`string`](/oaknut/ref/base_group/string)` & layoutAsset)` |  |
| *navigateBack* | `bool navigateBack()` |  |
| *onBackButtonClicked* | `void onBackButtonClicked()` |  |
| *onDidAppear* | `void onDidAppear(bool firstTime)` |  |
| *onDidDisappear* | `void onDidDisappear(bool lastTime)` |  |
| *onWillAppear* | `void onWillAppear(bool firstTime)` |  |
| *onWillDisappear* | `void onWillDisappear(bool lastTime)` |  |
| *onWindowAttached* | `void onWindowAttached()` |  |
| *onWindowDetached* | `void onWindowDetached()` |  |
| *requestScroll* | `void requestScroll(float dx, float dy)` |  |
| *setTitle* | `void setTitle(const `[`string`](/oaknut/ref/base_group/string)` & title)` |  |
| *setTitleView* | `void setTitleView(`[`View`](/oaknut/ref/views/View)` * titleView)` |  |
| *setView* | `void setView(`[`View`](/oaknut/ref/views/View)` * view)` |  |
| *~ViewController* | ` ~ViewController()` |  |
