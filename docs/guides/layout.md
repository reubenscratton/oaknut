---
layout: default
---
# Resources

Style and layout information may be declared in resource files (.res) in the assets
directory that gets built into the application bundle. Resource syntax
is similar to Json but much easier to read and edit. For sake of example let's
look at a simple `Label` declaration:

  {
    class: Label
    size: fill, wrap
    text: Hello world!
    background: #000
    forecolor: pink
    font-size: 24dp
  }

Important things to notice:

 - Nothing is in quotes.
 - Fields are separated by newlines rather than commas.
 - CSS-style color declarations are supported, including named web colors.
 - Android-style 'dp' measurements are supported.
 - The comma in the 'size' value indicates the value is an *array*, not
   text. In the particular case of 'size' an array of 2 elements means
   the first controls width and the second controls height. (A single value would
   mean width and height are the same and more than 2 elements is an error.)


Formally the syntax is:

   value = scalar|compound|array
   scalar = string|number|measurement|color|ref
   compound = '{' [identifier: value ...] '}'
   array = ['['] [value[, value...]] [']']
   string = ['"']...any text...['"']
   measurement = number['dp'|'sp'|'px']
   color = #xxx | #xxxxxx | #xxxxxxxx | 0xXXXXXXXX
   ref = $identifier

Newlines are significant in that they mark the end of the current scalar value,
unless that value is a quoted string which are allowed to contain newlines. A
quoted string may also include escape characters.

Commas also mark the end of the current scalar and imply that the currently
processed value is an array.



# Layout

Layout and Content size
=======================
All views have a "contentSize" property. For an ImageView the contentSize is the displayed size of the image, for a Label the content size is the size of the displayed text. If a view does not set its contentSize but does contain subviews, the contentSize is automatically set to encompass the furthest extent of child views.

If the contentSize is larger than the view's own size then it becomes scrollable. If the contentSize is smaller than the view's own size then the content is positioned with respect to the view's "gravity" property.

If a view's content size is invalidated it will only trigger a layout pass if either of the measurespecs are content-based. If the measurespecs are 'fill' or fixed-sized and don't depend on content, then updateContentSize() will be called immediately.



Size
----
Size may be a single value, in which case it applies to both width and height, or it may be an intrinsic array of two values (e.g. 'fill, 30dp').

A single size value has up to 3 components: a ref, a multiplier, and a constant. The 'ref' is a string: "fill", "wrap", "aspect", or the id of a previously-declared sibling view. If the ref is omitted but a multiplier is used then the parent view is assumed as the ref.

The 'multiplier' tells how to multiply the reference size (the default multiplier when a reference is specified is 1.0) and the constant part is added.

Some examples:

	Size value              ref      mult    constant        Meaning
        --------------          ---      ----    --------        -------
	wrap			content	 1.0          0		 Size will match view's content size.
	30dp			 -        0          30dp        30dp
        100% - 32dp             parent   1.0        -32dp        100% of parent size, - 32dp
	50% + 32dp		parent   0.5         32dp        50% of parent size, + 32dp
	view_id                 view_id	 1.0          0          Same size as view_id
	view_id(25%) - 8px	view_id	 0.25        -8px        25% of view_id size, minus 8px
	aspect(0.75)		self     0.75         0          Size is 75% of the other dimension

'fill' is a synonym for 'parent(100%)'.

NB: I don't like "fill" any more cos it implies filling the parent but if top-left isnt (0,0) then it will exceed parent bounds... occasionally this is counter intuitive. Maybe don't have "fill" or any other synonym for parent, i.e. you use "100%" etc.

There is potential for ambiguity when a multiplier or constant is given but not both: Should the simple value "2" be interpreted as a multiplier of 2x or as a constant of 2 pixels? It will be interpreted as a constant, unless the percentage notation was used in which case it is of course a multiplier.

Alignment
---------
Alignment values have up to 4 components: a ref, a ref multiplier, a self multiplier, and a constant. The ref is either the ID of a sibling view, or if omitted is the parent view.

top		align top edge to top of parent
top(id)		align top edge to id's top edge
