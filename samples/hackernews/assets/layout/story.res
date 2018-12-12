{
    class: StoryItemView
    size: fill, 64dp
    padding: 16dp, 8dp
    subviews: [
    {
        class: Label
        id: title
        style: $listview.item-title
        size: fill, wrap
        maxLines: 2
        align: left, top
    },
    {
        class: Label
        id: subtitle
        style: $listview.item-subtitle
        size: fill, wrap
        align: left, bottom
    }
    ]

}
