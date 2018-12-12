---
layout: default
---

# LayoutAnimation

```
class LayoutAnimation
    : public Animation
```


Animation that works on a View's position and updates the View's layout properties at the end, i.e.     

it is for persistent animations.     
[`LayoutAnimation`](/ref/views/LayoutAnimation)`* startHorizontal(`[`View`](/ref/views/View)`* view, ALIGNSPEC newAlignspec, int duration, InterpolateFunc interpolator)`<br>

[`LayoutAnimation`](/ref/views/LayoutAnimation)`* startVertical(`[`View`](/ref/views/View)`* view, ALIGNSPEC newAlignspec, int duration, InterpolateFunc interpolator)`<br>

[`LayoutAnimation`](/ref/views/LayoutAnimation)`* startPositional(`[`View`](/ref/views/View)`* view, ALIGNSPEC newAlignspecHorz, ALIGNSPEC newAlignspecVert, int duration, InterpolateFunc interpolator)`<br>


` LayoutAnimation(`[`View`](/ref/views/View)`* view, InterpolateFunc interpolator)`<br>

`void apply(float val)`<br>


