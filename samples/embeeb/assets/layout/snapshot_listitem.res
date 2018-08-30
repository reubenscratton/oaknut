{
    class: View
    width: fill_parent
    height: 64dp
    paddingHorz: 16dp
    paddingVert: 8dp
    subviews: [{
        class: ImageView
        id: image
        width: 72dp
        height: 52dp
        alignX: left
        alignY: center
        background: #333
        paddingHorz: 1dp
        paddingVert: 1dp
    }, {
        class: Label
        id: title
        style: $listview.item-title
        width: fill_parent
        height: wrap_content
        alignX: toRightOf, image, 8dp
        alignY: top
    }, {
        class: Label
        id: subtitle
        style: $listview.item-subtitle
        width: fill_parent
        height: wrap_content
        alignX: toRightOf, image, 8dp
        alignY: bottom
    }]
}

}
