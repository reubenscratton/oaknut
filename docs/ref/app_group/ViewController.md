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
## Events

| `void onWindowAttached()` | Called when this view controller is attached to the window. |
| `void onWindowDetached()` |  |
| `void onWillAppear(bool firstTime)` | Navigation events. |
| `void onDidAppear(bool firstTime)` |  |
| `void onWillDisappear(bool lastTime)` |  |
| `void onDidDisappear(bool lastTime)` |  |
| `void onBackButtonClicked()` | Called when the user navigates 'back'. |


## 



## 



## 

| `<a href="classstring.html">string</a> getTitle()` |  |
| `void setTitle(const <a href="classstring.html">string</a> & title)` |  |
| `void setTitleView(<a href="class_view.html">View</a> * titleView)` |  |
| `class ToolbarButton * addNavButton(bool rightSide, const <a href="classstring.html">string</a> & assetPath, std::function< void()> onClick)` |  |
| ` ViewController()` |  |
| ` ~ViewController()` |  |
| `<a href="class_view.html">View</a> * getView()` | Get the root view. |
| `void setView(<a href="class_view.html">View</a> * view)` | Set the root view. |
| `<a href="class_window.html">Window</a> * getWindow()` | Get the Window that the root view is attached to. |
| `<a href="class_view.html">View</a> * inflate(const <a href="classstring.html">string</a> & layoutAsset)` |  |
| `void bind(T *& rview, const <a href="classstring.html">string</a> & id)` |  |
| `void applySafeInsets(const EDGEINSETS & safeInsets)` |  |
| `bool navigateBack()` |  |
| `void requestScroll(float dx, float dy)` |  |


# Methods

| *onWindowAttached* |  `void <a href="todo">onWindowAttached</a>()` |  |
| *onWindowDetached* |  `void <a href="todo">onWindowDetached</a>()` |  |
| *onWillAppear* |  `void <a href="todo">onWillAppear</a>(bool firstTime)` |  |
| *onDidAppear* |  `void <a href="todo">onDidAppear</a>(bool firstTime)` |  |
| *onWillDisappear* |  `void <a href="todo">onWillDisappear</a>(bool lastTime)` |  |
| *onDidDisappear* |  `void <a href="todo">onDidDisappear</a>(bool lastTime)` |  |
| *onBackButtonClicked* |  `void <a href="todo">onBackButtonClicked</a>()` |  |
| *getTitle* |  `<a href="classstring.html">string</a> <a href="todo">getTitle</a>()` |  |
| *setTitle* |  `void <a href="todo">setTitle</a>(const <a href="classstring.html">string</a> & title)` |  |
| *setTitleView* |  `void <a href="todo">setTitleView</a>(<a href="class_view.html">View</a> * titleView)` |  |
| *addNavButton* |  `class ToolbarButton * <a href="todo">addNavButton</a>(bool rightSide, const <a href="classstring.html">string</a> & assetPath, std::function< void()> onClick)` |  |
| *ViewController* |  ` <a href="todo">ViewController</a>()` |  |
| *~ViewController* |  ` <a href="todo">~ViewController</a>()` |  |
| *getView* |  `<a href="class_view.html">View</a> * <a href="todo">getView</a>()` |  |
| *setView* |  `void <a href="todo">setView</a>(<a href="class_view.html">View</a> * view)` |  |
| *getWindow* |  `<a href="class_window.html">Window</a> * <a href="todo">getWindow</a>()` |  |
| *inflate* |  `<a href="class_view.html">View</a> * <a href="todo">inflate</a>(const <a href="classstring.html">string</a> & layoutAsset)` |  |
| *bind* |  `void <a href="todo">bind</a>(T *& rview, const <a href="classstring.html">string</a> & id)` |  |
| *applySafeInsets* |  `void <a href="todo">applySafeInsets</a>(const EDGEINSETS & safeInsets)` |  |
| *navigateBack* |  `bool <a href="todo">navigateBack</a>()` |  |
| *requestScroll* |  `void <a href="todo">requestScroll</a>(float dx, float dy)` |  |
