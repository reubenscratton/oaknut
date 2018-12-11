---
layout: default
---

# NavigationController

```
class NavigationController
    : public ViewController
```


Root view controller for typical mobile UX where there is a bar at the top of the screen and pages are pushed and popped in and out of view.     

    
| | |
|-|-|
|` NavigationController()`||
|`void pushViewController(`[`ViewController`](/ref/app_group/ViewController)` * vc)`||
|`void popViewController()`||
|`void onWindowAttached()`|Called when this view controller is attached to the window.|
|`void onWindowDetached()`||
|`bool navigateBack()`||
|`void requestScroll(float dx, float dy)`||
|`void applySafeInsets(const EDGEINSETS & safeInsets)`||
|`void applySafeInsetsToChild(`[`ViewController`](/ref/app_group/ViewController)` * childVC)`||


| | |
|-|-|
|`void startNavAnimation(`[`ViewController`](/ref/app_group/ViewController)` * incomingVC, AnimationState animationState)`||
|`void applyNavTransitionToViewController(`[`ViewController`](/ref/app_group/ViewController)` * vc, float val, bool incoming)`||
|`void onNavTransitionApply(float val)`||
|`void completeIncoming()`||


## Methods

| | |
|-|-|
| *NavigationController* | ` NavigationController()` |  |
| *pushViewController* | `void pushViewController(`[`ViewController`](/ref/app_group/ViewController)` * vc)` |  |
| *popViewController* | `void popViewController()` |  |
| *onWindowAttached* | `void onWindowAttached()` |  |
| *onWindowDetached* | `void onWindowDetached()` |  |
| *navigateBack* | `bool navigateBack()` |  |
| *requestScroll* | `void requestScroll(float dx, float dy)` |  |
| *applySafeInsets* | `void applySafeInsets(const EDGEINSETS & safeInsets)` |  |
| *applySafeInsetsToChild* | `void applySafeInsetsToChild(`[`ViewController`](/ref/app_group/ViewController)` * childVC)` |  |
| *startNavAnimation* | `void startNavAnimation(`[`ViewController`](/ref/app_group/ViewController)` * incomingVC, AnimationState animationState)` |  |
| *applyNavTransitionToViewController* | `void applyNavTransitionToViewController(`[`ViewController`](/ref/app_group/ViewController)` * vc, float val, bool incoming)` |  |
| *onNavTransitionApply* | `void onNavTransitionApply(float val)` |  |
| *completeIncoming* | `void completeIncoming()` |  |
