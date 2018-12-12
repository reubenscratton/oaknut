---
layout: default
---

# EditText

```
class EditText
    : public Label, 
public IKeyboardInputHandler, 
public ITextInputReceiver
```


A user-editable piece of text.     

    
### Properties

`void setInsertionPoint(int32_t newInsertionPoint)`<br>

`void setMaxLength(int32_t maxLength)`<br>

`void setActionType(ActionType actionType)`<br>


### Events

### Overrides

`bool applyStyleValue(const `[`string`](/ref/base_group/string)` & name, const `[`StyleValue`](/ref/app_group/StyleValue)`* value)`<br>Applies a single style value for the given attribute name.
Custom views should override this method to add support for custom attributes.

`void setPadding(EDGEINSETS padding)`<br>Set padding insets, i.e.
the space between the view rect and its content

`IKeyboardInputHandler* getKeyboardInputHandler()`<br>

`ITextInputReceiver* getTextInputReceiver()`<br>

`bool handleInputEvent(INPUTEVENT* event)`<br>

`void onStateChanged(STATESET changes)`<br>Called whenever state changes.

`void updateRenderOps()`<br>

`void layout(RECT constraint)`<br>Recalculate and apply this view's size and position (as well as all subviews).
Default implementation uses SIZESPEC and ALIGNSPEC members but derived types may override the default behaviour.

`void detachFromWindow()`<br>Called when the view is detached from the application Window.

`void setText(const AttributedString & text)`<br>

`void updateContentSize(SIZE constrainingSize)`<br>Called during measure() views should set the _contentSize property here.
The defaut implementation does nothing.


`void keyInputEvent(KeyboardInputEventType keyboardInputEventType, KeyboardInputSpecialKeyCode specialKeyCode, int osKeyCode, char32_t charCode)`<br>

`bool insertText(`[`string`](/ref/base_group/string)` text, int replaceStart, int replaceEnd)`<br>

`void deleteBackward()`<br>

`int getTextLength()`<br>

`int getSelectionStart()`<br>

`int getInsertionPoint()`<br>

[`string`](/ref/base_group/string)` textInRange(int start, int end)`<br>

`void setSelectedRange(int start, int end)`<br>

`SoftKeyboardType getSoftKeyboardType()`<br>

`ActionType getActionType()`<br>

`void handleActionPressed()`<br>


`void updateCursor()`<br>

`void updateClearButton()`<br>

`void moveCursor(int dx, int dy)`<br>

`void setInsertionPointReal(int32_t & newInsertionPoint)`<br>

`void notifySelectionChanged()`<br>


