---
layout: default
---

# StyleValue

```
class StyleValue

Represents a value in the global style namespace.     

Not instantiated directly, use `App::getStyleValue()` and friends to get one, then use one of the accessors `intVal()`, `stringVal()` to get to the raw value.Is alarmingly similar to the 'variant' type with which it should probably be unified.     
## 



## 



## 



## 

| ` StyleValue()` |  |
| ` StyleValue(const <a href="class_style_value.html">StyleValue</a> & )` |  |
| ` StyleValue(<a href="class_style_value.html">StyleValue</a> && )` |  |
| ` ~StyleValue()` |  |
| `<a href="class_style_value.html">StyleValue</a> & operator=(const <a href="class_style_value.html">StyleValue</a> & other)` |  |
| `bool isEmpty()` |  |
| `bool isNumeric()` |  |
| `bool isString()` |  |
| `bool isMeasurement()` |  |
| `bool isArray()` |  |
| `int intVal()` |  |
| `bool boolVal()` |  |
| `float floatVal()` |  |
| `<a href="classstring.html">string</a> stringVal()` |  |
| `measurement measurementVal()` |  |
| `<a href="class_c_o_l_o_r.html">COLOR</a> colorVal()` |  |
| `const vector< <a href="class_style_value.html">StyleValue</a> > & arrayVal()` |  |
| `const map< <a href="classstring.html">string</a>, <a href="class_style_value.html">StyleValue</a> > & compoundVal()` |  |
| `VECTOR4 cornerRadiiVal()` |  |
| `EDGEINSETS edgeInsetsVal()` |  |
| `float fontWeightVal()` |  |
| `const <a href="class_style_value.html">StyleValue</a> * get(const <a href="classstring.html">string</a> & keypath)` |  |
| `int intVal(const <a href="classstring.html">string</a> & name)` |  |
| `float floatVal(const <a href="classstring.html">string</a> & name)` |  |
| `<a href="classstring.html">string</a> stringVal(const <a href="classstring.html">string</a> & name)` |  |
| `const vector< <a href="class_style_value.html">StyleValue</a> > & arrayVal(const <a href="classstring.html">string</a> & name)` |  |
| `void importValues(const map< <a href="classstring.html">string</a>, <a href="class_style_value.html">StyleValue</a> > & values)` |  |
| `bool parse(class StringProcessor & it, int flags)` |  |


## 

| `void setType(Type newType)` |  |
| `void copyFrom(const <a href="class_style_value.html">StyleValue</a> * other)` |  |
| `const <a href="class_style_value.html">StyleValue</a> * select()` |  |
| `bool parseNumberOrMeasurement(StringProcessor & it)` |  |


# Methods

| *StyleValue* |  ` <a href="todo">StyleValue</a>()` |  |
| *StyleValue* |  ` <a href="todo">StyleValue</a>(const <a href="class_style_value.html">StyleValue</a> & )` |  |
| *StyleValue* |  ` <a href="todo">StyleValue</a>(<a href="class_style_value.html">StyleValue</a> && )` |  |
| *~StyleValue* |  ` <a href="todo">~StyleValue</a>()` |  |
| *operator=* |  `<a href="class_style_value.html">StyleValue</a> & <a href="todo">operator=</a>(const <a href="class_style_value.html">StyleValue</a> & other)` |  |
| *isEmpty* |  `bool <a href="todo">isEmpty</a>()` |  |
| *isNumeric* |  `bool <a href="todo">isNumeric</a>()` |  |
| *isString* |  `bool <a href="todo">isString</a>()` |  |
| *isMeasurement* |  `bool <a href="todo">isMeasurement</a>()` |  |
| *isArray* |  `bool <a href="todo">isArray</a>()` |  |
| *intVal* |  `int <a href="todo">intVal</a>()` |  |
| *boolVal* |  `bool <a href="todo">boolVal</a>()` |  |
| *floatVal* |  `float <a href="todo">floatVal</a>()` |  |
| *stringVal* |  `<a href="classstring.html">string</a> <a href="todo">stringVal</a>()` |  |
| *measurementVal* |  `measurement <a href="todo">measurementVal</a>()` |  |
| *colorVal* |  `<a href="class_c_o_l_o_r.html">COLOR</a> <a href="todo">colorVal</a>()` |  |
| *arrayVal* |  `const vector< <a href="class_style_value.html">StyleValue</a> > & <a href="todo">arrayVal</a>()` |  |
| *compoundVal* |  `const map< <a href="classstring.html">string</a>, <a href="class_style_value.html">StyleValue</a> > & <a href="todo">compoundVal</a>()` |  |
| *cornerRadiiVal* |  `VECTOR4 <a href="todo">cornerRadiiVal</a>()` |  |
| *edgeInsetsVal* |  `EDGEINSETS <a href="todo">edgeInsetsVal</a>()` |  |
| *fontWeightVal* |  `float <a href="todo">fontWeightVal</a>()` |  |
| *get* |  `const <a href="class_style_value.html">StyleValue</a> * <a href="todo">get</a>(const <a href="classstring.html">string</a> & keypath)` |  |
| *intVal* |  `int <a href="todo">intVal</a>(const <a href="classstring.html">string</a> & name)` |  |
| *floatVal* |  `float <a href="todo">floatVal</a>(const <a href="classstring.html">string</a> & name)` |  |
| *stringVal* |  `<a href="classstring.html">string</a> <a href="todo">stringVal</a>(const <a href="classstring.html">string</a> & name)` |  |
| *arrayVal* |  `const vector< <a href="class_style_value.html">StyleValue</a> > & <a href="todo">arrayVal</a>(const <a href="classstring.html">string</a> & name)` |  |
| *importValues* |  `void <a href="todo">importValues</a>(const map< <a href="classstring.html">string</a>, <a href="class_style_value.html">StyleValue</a> > & values)` |  |
| *parse* |  `bool <a href="todo">parse</a>(class StringProcessor & it, int flags)` |  |
| *setType* |  `void <a href="todo">setType</a>(Type newType)` |  |
| *copyFrom* |  `void <a href="todo">copyFrom</a>(const <a href="class_style_value.html">StyleValue</a> * other)` |  |
| *select* |  `const <a href="class_style_value.html">StyleValue</a> * <a href="todo">select</a>()` |  |
| *parseNumberOrMeasurement* |  `bool <a href="todo">parseNumberOrMeasurement</a>(StringProcessor & it)` |  |
