---
layout: default
---

# NavigationController

```
class NavigationController
    : public ViewController
```


Root view controller for typical mobile UX where there is a bar at the top of the screen and pages are pushed and popped in and out of view.     

    
## 



## 



## 

| ` NavigationController()` |  |
| `void pushViewController(<a href="class_view_controller.html">ViewController</a> * vc)` |  |
| `void popViewController()` |  |
| `void onWindowAttached()` | Called when this view controller is attached to the window. |
| `void onWindowDetached()` |  |
| `bool navigateBack()` |  |
| `void requestScroll(float dx, float dy)` |  |
| `void applySafeInsets(const EDGEINSETS & safeInsets)` |  |
| `void applySafeInsetsToChild(<a href="class_view_controller.html">ViewController</a> * childVC)` |  |


## 

| `void startNavAnimation(<a href="class_view_controller.html">ViewController</a> * incomingVC, AnimationState animationState)` |  |
| `void applyNavTransitionToViewController(<a href="class_view_controller.html">ViewController</a> * vc, float val, bool incoming)` |  |
| `void onNavTransitionApply(float val)` |  |
| `void completeIncoming()` |  |


# Methods

| *NavigationController* |  ` <a href="todo">NavigationController</a>()` |  |
| *pushViewController* |  `void <a href="todo">pushViewController</a>(<a href="class_view_controller.html">ViewController</a> * vc)` |  |
| *popViewController* |  `void <a href="todo">popViewController</a>()` |  |
| *onWindowAttached* |  `void <a href="todo">onWindowAttached</a>()` |  |
| *onWindowDetached* |  `void <a href="todo">onWindowDetached</a>()` |  |
| *navigateBack* |  `bool <a href="todo">navigateBack</a>()` |  |
| *requestScroll* |  `void <a href="todo">requestScroll</a>(float dx, float dy)` |  |
| *applySafeInsets* |  `void <a href="todo">applySafeInsets</a>(const EDGEINSETS & safeInsets)` |  |
| *applySafeInsetsToChild* |  `void <a href="todo">applySafeInsetsToChild</a>(<a href="class_view_controller.html">ViewController</a> * childVC)` |  |
| *startNavAnimation* |  `void <a href="todo">startNavAnimation</a>(<a href="class_view_controller.html">ViewController</a> * incomingVC, AnimationState animationState)` |  |
| *applyNavTransitionToViewController* |  `void <a href="todo">applyNavTransitionToViewController</a>(<a href="class_view_controller.html">ViewController</a> * vc, float val, bool incoming)` |  |
| *onNavTransitionApply* |  `void <a href="todo">onNavTransitionApply</a>(float val)` |  |
| *completeIncoming* |  `void <a href="todo">completeIncoming</a>()` |  |
