---
layout: default
---

# ActionSheet

```
class ActionSheet
    : public ViewController
```


A simple port of the iOS ActionSheet.     

    
| | |
|-|-|
|` ActionSheet()`||
|`void addAction(const `[`string`](/ref/base_group/string)` & action, std::function< void()> onClick)`||
|`void addCancelButton()`||
|`void setTitle(const `[`string`](/ref/base_group/string)` & title)`||
|`void onWindowAttached()`|Called when this view controller is attached to the window.|


| | |
|-|-|
|`void dismissWithAction(std::function< void()> action)`||


## Methods

| | |
|-|-|
| *ActionSheet* | ` ActionSheet()` |  |
| *addAction* | `void addAction(const `[`string`](/ref/base_group/string)` & action, std::function< void()> onClick)` |  |
| *addCancelButton* | `void addCancelButton()` |  |
| *setTitle* | `void setTitle(const `[`string`](/ref/base_group/string)` & title)` |  |
| *onWindowAttached* | `void onWindowAttached()` |  |
| *dismissWithAction* | `void dismissWithAction(std::function< void()> action)` |  |
