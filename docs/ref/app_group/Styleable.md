---
layout: default
---

# Styleable

```
class Styleable
    : public Object
```


Abstract base class for objects that can have style values applied.     

In practice this means Views, but also TextRenderer.     
`void applyStyle(const `[`string`](/ref/base_group/string)` & style)`<br>

`void applyStyle(const class `[`StyleValue`](/ref/app_group/StyleValue)` & value)`<br>


`bool applyStyleValue(const `[`string`](/ref/base_group/string)` & name, const `[`StyleValue`](/ref/app_group/StyleValue)`* value)`<br>Applies a single style value for the given attribute name.
Custom views should override this method to add support for custom attributes.


