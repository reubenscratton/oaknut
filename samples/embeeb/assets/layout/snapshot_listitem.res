{
    class: View
    size: fill,64dp
    padding: 16dp, 8dp
    subviews: [
    {
        class: ImageView
        id: image
        size: 72dp,52dp
        align: left, center
        background: #333
        padding: 1dp
    },
    {
        class: Label
        id: title
        style: $listview.item-title
        size: fill,wrap
        align: toRightOf(image 8dp), top
    },
    {
        class: Label
        id: subtitle
        style: $listview.item-subtitle
        size: fill,wrap
        align: toRightOf(image 8dp), bottom
    }
    ]
}

}
