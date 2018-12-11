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
## 

| `void applyStyle(const <a href="classstring.html">string</a> & style)` |  |
| `void applyStyle(const class <a href="class_style_value.html">StyleValue</a> & value)` |  |


## 

| `bool applyStyleValue(const <a href="classstring.html">string</a> & name, const <a href="class_style_value.html">StyleValue</a> * value)` | Applies a single style value for the given attribute name. |


# Methods

| *applyStyle* |  `void <a href="todo">applyStyle</a>(const <a href="classstring.html">string</a> & style)` |  |
| *applyStyle* |  `void <a href="todo">applyStyle</a>(const class <a href="class_style_value.html">StyleValue</a> & value)` |  |
| *applyStyleValue* |  `bool <a href="todo">applyStyleValue</a>(const <a href="classstring.html">string</a> & name, const <a href="class_style_value.html">StyleValue</a> * value)` | Custom views should override this method to add support for custom attributes. |
