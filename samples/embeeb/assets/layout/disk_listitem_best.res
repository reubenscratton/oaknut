{
    class: View
    size: fill, 64dp
    padding: 16dp, 8dp
    subviews: [{
        class: Label
        id: number
        style: $listview.item-title
        size: 52dp, wrap_content
        align: left, center
        gravityX: right
        font-size: 32dp
      },{
        class: ImageView
        id: image
        size: 40dp, 52dp
        align: toRightOf(number 8dp), center
        background: #333
        padding: 1dp
      }, {
        class: Label
        id: title
        style: $listview.item-title
        size: fill, wrap
        align: toRightOf(image 8dp), top
      }, {
        class: Label
        id: subtitle
        style: $listview.item-subtitle
        size: fill, wrap
        align: toRightOf(image 8dp), bottom
    }]
}

}
