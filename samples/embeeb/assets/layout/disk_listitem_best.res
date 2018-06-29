{
View: {
    width: fill_parent
    height: 64dp
    paddingHorz: 16dp
    paddingVert: 8dp
    Label: {
        id: number
        style: $listview.item-title
        width: 52dp
        height: wrap_content
        alignX: left
        alignY: center
        gravityX: right
        font-size: 32dp
    }
    ImageView: {
        id: image
        width: 40dp
        height: 52dp
        alignX: toRightOf(number, 8dp)
        alignY: center
        background: 0xff333333
        paddingHorz: 1dp
        paddingVert: 1dp
    }
    Label: {
        id: title
        style: $listview.item-title
        width: fill_parent
        height: wrap_content
        alignX: toRightOf(image, 8dp)
        alignY: top
    }
    Label: {
        id: subtitle
        style: $listview.item-subtitle
        width: fill_parent
        height: wrap_content
        alignX: toRightOf(image, 8dp)
        alignY: bottom
    }
}

}
