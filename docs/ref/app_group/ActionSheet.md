---
layout: default
---

# ActionSheet

```
class ActionSheet
    : public ViewController
```


A simple port of the iOS ActionSheet.     

    
` ActionSheet()`<br>

`void addAction(const `[`string`](/oaknut/ref/base_group/string)` & action, std::function< void()> onClick)`<br>

`void addCancelButton()`<br>

`void setTitle(const `[`string`](/oaknut/ref/base_group/string)` & title)`<br>

`void onWindowAttached()`<br>Called when this view controller is attached to the window.



`void dismissWithAction(std::function< void()> action)`<br>



## Methods

| | |
|-|-|
| *ActionSheet* | ` ActionSheet()` |  |
| *addAction* | `void addAction(const `[`string`](/oaknut/ref/base_group/string)` & action, std::function< void()> onClick)` |  |
| *addCancelButton* | `void addCancelButton()` |  |
| *dismissWithAction* | `void dismissWithAction(std::function< void()> action)` |  |
| *onWindowAttached* | `void onWindowAttached()` |  |
| *setTitle* | `void setTitle(const `[`string`](/oaknut/ref/base_group/string)` & title)` |  |
