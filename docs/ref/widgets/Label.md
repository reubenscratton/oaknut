---
layout: default
---

# Label

```
class Label
    : public View
```


Displays a piece of text, potentially with attributes.     

    
### Properties

`const AttributedString & getText()`<br>

`void setText(const AttributedString & text)`<br>

`void setTextColor(`[`COLOR`](/ref/graphics_group/COLOR)` color)`<br>

`void setFont(Font* font)`<br>

`void setFontName(const `[`string`](/ref/base_group/string)` & fontName)`<br>

`void setMaxLines(int maxLines)`<br>

`const Attribute* getAttribute(int32_t pos, Attribute::Type type)`<br>


### Overrides

`bool applyStyleValue(const `[`string`](/ref/base_group/string)` & name, const `[`StyleValue`](/ref/app_group/StyleValue)`* value)`<br>Applies a single style value for the given attribute name.
Custom views should override this method to add support for custom attributes.

`void invalidateContentSize()`<br>Mark the view's contentSize as invalid and call setNeedsLayout()

`void layout(RECT constraint)`<br>Recalculate and apply this view's size and position (as well as all subviews).
Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour.

`void onEffectiveTintColorChanged()`<br>

`void setContentOffset(POINT contentOffset)`<br>

`void setGravity(GRAVITY gravity)`<br>Set the gravity flags.
Gravity affects how the content is aligned within the view rect when the view rect size exceeds the content size.

`void updateContentSize(SIZE constrainingSize)`<br>Called during measure() views should set the _contentSize property here.
The defaut implementation does nothing.

`void updateRenderOps()`<br>


