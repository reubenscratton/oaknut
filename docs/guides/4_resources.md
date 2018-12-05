---
layout: default
---
# Resources

Style and layout information may be declared in resource files (.res) in the assets
directory that gets built into the application bundle. Resource syntax
is similar to Json but easier to read and edit. For sake of example let's
look at a simple `Label` layout declaration:

````
  {
    class: Label
    size: fill, wrap
    text: Hello world!
    background: #000
    forecolor: pink
    font-size: 24dp
  }
````
Things to note:

 - Nothing is in quotes.
 - Fields are separated by newlines rather than commas.
 - CSS-style color declarations are supported, including named web colors.
 - Android-style 'dp' measurements are supported.
 - The comma in the 'size' value indicates the value is an *array*, not
   text. In the particular case of 'size' an array of 2 elements means
   the first controls width and the second controls height. (A single value would
   mean width and height are the same and more than 2 elements is an error.)


### Quoted and multiline string values

Putting a string value in double quotes means you can use newlines and commas
which are normally reserved for delimiting values

````
  text: "Here is, ahem, a
         multiline value. It extends
         onto a third line."
````
Quoted strings also support the standard whitespace, quote, and Unicode escapes:
````
  text: "Line 1\n\"Line 2\"\nLine 3"
````

### Qualifiers

Resource value names may be qualified with one or more suffixes that limit the
value to particular runtime properties. This is similar to Android's
resource system except here the qualifiers apply to individual values rather than
directories of resource files. For example, to supply localised string values
for a particular label you could do this:
````
  {
    class: Label
    text@en_GB: "Hello"
    text@en_US: "Hi"
    text@fr: "Bonjour"
  }
````

Qualifiers exist for a wide range of runtime properties:

| Type             | Qualifiers   |
|------------------|--------------|
|Platform          |  `android`, `ios`, `web`, `mac`, `windows`, `linux` |
|Languages         |  `en`, `fr`, `es`, etc. See [ISO 639-2 two-digit codes](http://www.loc.gov/standards/iso639-2/php/code_list.php) |
|Language & region | `en_GB`, `en_CA`, etc, i.e. the ISO-639-1 country code combined with a [ISO-3166-1 alpha 2 region code](https://www.iso.org/obp/ui/#iso:pub:PUB500001:en).
|Screen orientation| `port`, `land` |
|Screen size       | `s320dp`, `l480dp`, etc ('s' prefix means 'shorter', 'l' means 'longer') |
|Screen density    | `160dpi`, `320dpi`, `1x`, `2x`, `3x`, `4x` |


In the presence of multiple valid qualified values the runtime will favour the
most specific match. For a screen whose longer dimension is 480dp the
qualifiers `l320dp` and `l480dp` are both valid, but since `l480dp` is more
specific it will be chosen ahead of `l320dp`.

### Syntax

In something that's vaguely close to BNF :
````
   <value> ::= <scalar> | <compound> | <array>
   <scalar> ::= <string> | <number> | <measurement> | <color> | <ref>
   <compound> ::= "{" [<identifier> ":" <value> ["\n" ...]] "}"
   <array> ::= ["["] [<value>["," ...]] ["]"]
   <string> ::= ["""]...any text...["""]
   <measurement> ::= <number>["dp" | "sp" | "px"]
   <color> ::= #xxx | #xxxxxx | #xxxxxxxx | <named web color>
   <ref> ::= "$" <identifier>
````
