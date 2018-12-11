---
layout: default
---

# NavigationController

```
class NavigationController
    : public ViewController
```


Root view controller for typical mobile UX where there is a bar at the top of the screen and pages are pushed and popped in and out of view.     

    
` NavigationController()`<br>
`void pushViewController(`[`ViewController`](/oaknut/ref/app_group/ViewController)`* vc)`<br>
`void popViewController()`<br>
`void onWindowAttached()`<br>Called when this view controller is attached to the window.

`void onWindowDetached()`<br>
`bool navigateBack()`<br>
`void requestScroll(float dx, float dy)`<br>
`void applySafeInsets(const EDGEINSETS & safeInsets)`<br>
`void applySafeInsetsToChild(`[`ViewController`](/oaknut/ref/app_group/ViewController)`* childVC)`<br>

`void startNavAnimation(`[`ViewController`](/oaknut/ref/app_group/ViewController)`* incomingVC, AnimationState animationState)`<br>
`void applyNavTransitionToViewController(`[`ViewController`](/oaknut/ref/app_group/ViewController)`* vc, float val, bool incoming)`<br>
`void onNavTransitionApply(float val)`<br>
`void completeIncoming()`<br>

