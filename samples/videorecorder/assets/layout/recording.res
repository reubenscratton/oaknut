{
class: View
size: fill, 80dp
subviews: [
    {
    id: label
    class: Label
    size: 60dp, fill
    alignX: left
    font-weight: bold
    font-size:32dp
    forecolor: white
    gravity: center
    },
    {
    id: redo
    class: ToolbarButton
    size: 60dp, fill
    alignX: right
    image: images/redo.png
    tint: white
    visibility: hidden
    }
]
}
