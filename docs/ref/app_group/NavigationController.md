---
layout: default
---

# NavigationController

```
class NavigationController
    : public ViewController
```


Root view controller for typical mobile UX where there is a bar at the top of the screen and pages are pushed and popped in and out of view.     

    
### Overrides

`void onWindowAttached()`<br>Called when this view controller is attached to a window.

`void onWindowDetached()`<br>Called when this view controller is detached from a window.

`bool navigateBack()`<br>

`void requestScroll(float dx, float dy)`<br>

`void applySafeInsets(const EDGEINSETS & safeInsets)`<br>


`void pushViewController(`[`ViewController`](/ref/app_group/ViewController)`* vc)`<br>Push a new child `ViewController` on top of the stack.

`void popViewController()`<br>Pop the current topmost child `ViewController` off the stack.


`void startNavAnimation(`[`ViewController`](/ref/app_group/ViewController)`* incomingVC, AnimationState animationState)`<br>

`void applyNavTransitionToViewController(`[`ViewController`](/ref/app_group/ViewController)`* vc, float val, bool incoming)`<br>

`void onNavTransitionApply(float val)`<br>

`void completeIncoming()`<br>

`void applySafeInsetsToChild(`[`ViewController`](/ref/app_group/ViewController)`* childVC)`<br>


