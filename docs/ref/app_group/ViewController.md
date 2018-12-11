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

| | |
|-|-|
|`void onWindowAttached()`|Called when this view controller is attached to the window.|
|`void onWindowDetached()`||
|`void onWillAppear(bool firstTime)`|Navigation events.|
|`void onDidAppear(bool firstTime)`||
|`void onWillDisappear(bool lastTime)`||
|`void onDidDisappear(bool lastTime)`||
|`void onBackButtonClicked()`|Called when the user navigates 'back'.|


| | |
|-|-|
|[`string`](/ref/base_group/string)` getTitle()`||
|`void setTitle(const `[`string`](/ref/base_group/string)` & title)`||
|`void setTitleView(`[`View`](/ref/views/View)` * titleView)`||
|`class ToolbarButton * addNavButton(bool rightSide, const `[`string`](/ref/base_group/string)` & assetPath, std::function< void()> onClick)`||
|` ViewController()`||
|` ~ViewController()`||
|[`View`](/ref/views/View)` * getView()`|Get the root view.|
|`void setView(`[`View`](/ref/views/View)` * view)`|Set the root view.|
|[`Window`](/ref/views/Window)` * getWindow()`|Get the Window that the root view is attached to.|
|[`View`](/ref/views/View)` * inflate(const `[`string`](/ref/base_group/string)` & layoutAsset)`||
|`void bind(T *& rview, const `[`string`](/ref/base_group/string)` & id)`||
|`void applySafeInsets(const EDGEINSETS & safeInsets)`||
|`bool navigateBack()`||
|`void requestScroll(float dx, float dy)`||


## Methods

| | |
|-|-|
| *onWindowAttached* | `void onWindowAttached()` |  |
| *onWindowDetached* | `void onWindowDetached()` |  |
| *onWillAppear* | `void onWillAppear(bool firstTime)` |  |
| *onDidAppear* | `void onDidAppear(bool firstTime)` |  |
| *onWillDisappear* | `void onWillDisappear(bool lastTime)` |  |
| *onDidDisappear* | `void onDidDisappear(bool lastTime)` |  |
| *onBackButtonClicked* | `void onBackButtonClicked()` |  |
| *getTitle* | [`string`](/ref/base_group/string)` getTitle()` |  |
| *setTitle* | `void setTitle(const `[`string`](/ref/base_group/string)` & title)` |  |
| *setTitleView* | `void setTitleView(`[`View`](/ref/views/View)` * titleView)` |  |
| *addNavButton* | `class ToolbarButton * addNavButton(bool rightSide, const `[`string`](/ref/base_group/string)` & assetPath, std::function< void()> onClick)` |  |
| *ViewController* | ` ViewController()` |  |
| *~ViewController* | ` ~ViewController()` |  |
| *getView* | [`View`](/ref/views/View)` * getView()` |  |
| *setView* | `void setView(`[`View`](/ref/views/View)` * view)` |  |
| *getWindow* | [`Window`](/ref/views/Window)` * getWindow()` |  |
| *inflate* | [`View`](/ref/views/View)` * inflate(const `[`string`](/ref/base_group/string)` & layoutAsset)` |  |
| *bind* | `void bind(T *& rview, const `[`string`](/ref/base_group/string)` & id)` |  |
| *applySafeInsets* | `void applySafeInsets(const EDGEINSETS & safeInsets)` |  |
| *navigateBack* | `bool navigateBack()` |  |
| *requestScroll* | `void requestScroll(float dx, float dy)` |  |
