{
    class: StoryItemView
    width: fill_parent
    height: 64dp
    paddingHorz: 16dp
    paddingVert: 8dp
    subviews: [{
        class: Label
        id: title
        style: $listview.item-title
        width: fill_parent
        height: wrap_content
        maxLines: 2
alignX: left
        alignY: top
    },{
        class: Label
        id: subtitle
        style: $listview.item-subtitle
        width: fill_parent
        height: wrap_content
alignX: left
        alignY: bottom
    }]

}
