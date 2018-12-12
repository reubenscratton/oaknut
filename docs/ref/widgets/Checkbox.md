---
layout: default
---

# Checkbox

```
class Checkbox
    : public ImageView
```


A checkbox is an ImageView that toggles it's `STATE_CHECKED` bit when tapped.     

    
` Checkbox()`<br>


`bool handleInputEvent(INPUTEVENT* event)`<br>

`void onStateChanged(STATESET changes)`<br>Called whenever state changes.


