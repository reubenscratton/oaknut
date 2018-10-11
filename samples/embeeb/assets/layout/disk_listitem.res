{
    class: View
    size: fill
    padding: 16dp, 8dp
    subviews: [
    {
        class: ImageView
        id: image
        size: 40dp, 52dp
        align: left, center
        contentMode: aspectFit
        gravity: center
        background: #333
        padding: 1dp
    },
    {
        class: Label
        id: title
        style: $listview.item-title
        size: 100%-56dp, wrap
        align: toRightOf(image 8dp), top
    },
    {
        class: Label
        id: subtitle
        style: $listview.item-subtitle
        size: 100%-56dp, wrap
        align: toRightOf(image 8dp), bottom
    }
    ]

}
