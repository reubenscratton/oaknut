---
layout: default
---

# ActionSheet

```
class ActionSheet
    : public ViewController
```


A simple port of the iOS ActionSheet.     

    
`void addAction(const `[`string`](/ref/base_group/string)` & action, std::function< void()> onClick)`<br>Add a text button with a click handler function.

`void addCancelButton()`<br>Add the standard 'Cancel' button.

`void setTitle(const `[`string`](/ref/base_group/string)` & title)`<br>

`void onWindowAttached()`<br>Called when this view controller is attached to a window.


`void dismissWithAction(std::function< void()> action)`<br>


