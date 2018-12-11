---
layout: default
---

# StyleValue

```
class StyleValue

Represents a value in the global style namespace.     

Not instantiated directly, use `App::getStyleValue()` and friends to get one, then use one of the accessors `intVal()`, `stringVal()` to get to the raw value.Is alarmingly similar to the 'variant' type with which it should probably be unified.     
| | |
|-|-|
|` StyleValue()`||
|` StyleValue(const `[`StyleValue`](/ref/app_group/StyleValue)` & )`||
|` StyleValue(`[`StyleValue`](/ref/app_group/StyleValue)` && )`||
|` ~StyleValue()`||
|[`StyleValue`](/ref/app_group/StyleValue)` & operator=(const `[`StyleValue`](/ref/app_group/StyleValue)` & other)`||
|`bool isEmpty()`||
|`bool isNumeric()`||
|`bool isString()`||
|`bool isMeasurement()`||
|`bool isArray()`||
|`int intVal()`||
|`bool boolVal()`||
|`float floatVal()`||
|[`string`](/ref/base_group/string)` stringVal()`||
|`measurement measurementVal()`||
|[`COLOR`](/ref/graphics_group/COLOR)` colorVal()`||
|`const vector< `[`StyleValue`](/ref/app_group/StyleValue)` > & arrayVal()`||
|`const map< `[`string`](/ref/base_group/string)`, `[`StyleValue`](/ref/app_group/StyleValue)` > & compoundVal()`||
|`VECTOR4 cornerRadiiVal()`||
|`EDGEINSETS edgeInsetsVal()`||
|`float fontWeightVal()`||
|`const `[`StyleValue`](/ref/app_group/StyleValue)` * get(const `[`string`](/ref/base_group/string)` & keypath)`||
|`int intVal(const `[`string`](/ref/base_group/string)` & name)`||
|`float floatVal(const `[`string`](/ref/base_group/string)` & name)`||
|[`string`](/ref/base_group/string)` stringVal(const `[`string`](/ref/base_group/string)` & name)`||
|`const vector< `[`StyleValue`](/ref/app_group/StyleValue)` > & arrayVal(const `[`string`](/ref/base_group/string)` & name)`||
|`void importValues(const map< `[`string`](/ref/base_group/string)`, `[`StyleValue`](/ref/app_group/StyleValue)` > & values)`||
|`bool parse(class StringProcessor & it, int flags)`||


| | |
|-|-|
|`void setType(Type newType)`||
|`void copyFrom(const `[`StyleValue`](/ref/app_group/StyleValue)` * other)`||
|`const `[`StyleValue`](/ref/app_group/StyleValue)` * select()`||
|`bool parseNumberOrMeasurement(StringProcessor & it)`||


## Methods

| | |
|-|-|
| *StyleValue* | ` StyleValue()` |  |
| *StyleValue* | ` StyleValue(const `[`StyleValue`](/ref/app_group/StyleValue)` & )` |  |
| *StyleValue* | ` StyleValue(`[`StyleValue`](/ref/app_group/StyleValue)` && )` |  |
| *~StyleValue* | ` ~StyleValue()` |  |
| *operator=* | [`StyleValue`](/ref/app_group/StyleValue)` & operator=(const `[`StyleValue`](/ref/app_group/StyleValue)` & other)` |  |
| *isEmpty* | `bool isEmpty()` |  |
| *isNumeric* | `bool isNumeric()` |  |
| *isString* | `bool isString()` |  |
| *isMeasurement* | `bool isMeasurement()` |  |
| *isArray* | `bool isArray()` |  |
| *intVal* | `int intVal()` |  |
| *boolVal* | `bool boolVal()` |  |
| *floatVal* | `float floatVal()` |  |
| *stringVal* | [`string`](/ref/base_group/string)` stringVal()` |  |
| *measurementVal* | `measurement measurementVal()` |  |
| *colorVal* | [`COLOR`](/ref/graphics_group/COLOR)` colorVal()` |  |
| *arrayVal* | `const vector< `[`StyleValue`](/ref/app_group/StyleValue)` > & arrayVal()` |  |
| *compoundVal* | `const map< `[`string`](/ref/base_group/string)`, `[`StyleValue`](/ref/app_group/StyleValue)` > & compoundVal()` |  |
| *cornerRadiiVal* | `VECTOR4 cornerRadiiVal()` |  |
| *edgeInsetsVal* | `EDGEINSETS edgeInsetsVal()` |  |
| *fontWeightVal* | `float fontWeightVal()` |  |
| *get* | `const `[`StyleValue`](/ref/app_group/StyleValue)` * get(const `[`string`](/ref/base_group/string)` & keypath)` |  |
| *intVal* | `int intVal(const `[`string`](/ref/base_group/string)` & name)` |  |
| *floatVal* | `float floatVal(const `[`string`](/ref/base_group/string)` & name)` |  |
| *stringVal* | [`string`](/ref/base_group/string)` stringVal(const `[`string`](/ref/base_group/string)` & name)` |  |
| *arrayVal* | `const vector< `[`StyleValue`](/ref/app_group/StyleValue)` > & arrayVal(const `[`string`](/ref/base_group/string)` & name)` |  |
| *importValues* | `void importValues(const map< `[`string`](/ref/base_group/string)`, `[`StyleValue`](/ref/app_group/StyleValue)` > & values)` |  |
| *parse* | `bool parse(class StringProcessor & it, int flags)` |  |
| *setType* | `void setType(Type newType)` |  |
| *copyFrom* | `void copyFrom(const `[`StyleValue`](/ref/app_group/StyleValue)` * other)` |  |
| *select* | `const `[`StyleValue`](/ref/app_group/StyleValue)` * select()` |  |
| *parseNumberOrMeasurement* | `bool parseNumberOrMeasurement(StringProcessor & it)` |  |
