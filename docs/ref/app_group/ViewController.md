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
### Views & Window

[`View`](/ref/views/View)`* inflate(const `[`string`](/ref/base_group/string)` & layoutAsset)`<br>Inflate the given layout asset and set it as the root view.

`void bind(T*& rview, const `[`string`](/ref/base_group/string)` & id)`<br>Find a subview by ID and store it in the given variable, usually member data.

[`View`](/ref/views/View)`* getView()`<br>Get the root view.

`void setView(`[`View`](/ref/views/View)`* view)`<br>Set the root view.

[`Window`](/ref/views/Window)`* getWindow()`<br>Get the Window that the root view is attached to.


### Navigation

[`string`](/ref/base_group/string)` getTitle()`<br>

`void setTitle(const `[`string`](/ref/base_group/string)` & title)`<br>

`void setTitleView(`[`View`](/ref/views/View)`* titleView)`<br>

`class `[`ToolbarButton`](/ref/widgets/ToolbarButton)`* addNavButton(bool rightSide, const `[`string`](/ref/base_group/string)` & assetPath, std::function< void()> onClick)`<br>


### Events

`void onWindowAttached()`<br>Called when this view controller is attached to a window.

`void onWindowDetached()`<br>Called when this view controller is detached from a window.

`void onWillAppear(bool firstTime)`<br>Called when this view controller is about to become the current top in a NavigationController.

`void onDidAppear(bool firstTime)`<br>Called when this view controller has become current top in a NavigationController.

`void onWillDisappear(bool lastTime)`<br>Called when this view controller is about to stop being current top in a NavigationController.

`void onDidDisappear(bool lastTime)`<br>Called when this view controller has stopped being current top in a NavigationController.

`void onBackButtonClicked()`<br>Called when the user navigates 'back'.


### Misc

`void applySafeInsets(const EDGEINSETS & safeInsets)`<br>

`bool navigateBack()`<br>

`void requestScroll(float dx, float dy)`<br>


