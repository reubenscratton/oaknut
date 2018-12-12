---
layout: default
---

# Animation

```
class Animation
    : public Object
```


Animation base class.     

Animations are associated with a View and are owned and updated by the Window attached to the View.     
### Construction

` Animation(`[`View`](/ref/views/View)`* view)`<br>

` Animation(`[`View`](/ref/views/View)`* view, float fromVal, float toVal)`<br>


### Starting & Stopping

`void start(int duration)`<br>

`void start(int duration, int delay)`<br>

`void stop()`<br>

`void pause()`<br>

`void unpause()`<br>


### Interpolater

`void setInterpolater(InterpolateFunc interpolator)`<br>

`InterpolateFunc getInterpolater()`<br>


### Interpolation functions

`float linear(float t, float b, float c, float d)`<br>

`float strongEaseInOut(float t, float b, float c, float d)`<br>

`float regularEaseIn(float t, float b, float c, float d)`<br>

`float regularEaseInOut(float t, float b, float c, float d)`<br>

`float easeOut(float t, float b, float c, float d)`<br>

`float strongEaseIn(float t, float b, float c, float d)`<br>

`float strongEaseOut(float t, float b, float c, float d)`<br>

`float nowhere(float t, float b, float c, float d)`<br>

`float bounceEaseOut(float t, float b, float c, float d)`<br>

`float bounceEaseIn(float t, float b, float c, float d)`<br>

`float bounceEaseInOut(float t, float b, float c, float d)`<br>

`float elasticEaseIn(float t, float b, float c, float d)`<br>

`float elasticEaseOut(float t, float b, float c, float d)`<br>


### Handlers

[`Animation`](/ref/views/Animation)`* start(`[`View`](/ref/views/View)`* view, int duration, std::function< void(float)> callback, InterpolateFunc interpolater)`<br>


`bool tick(TIMESTAMP now)`<br>

`void apply(float val)`<br>


