{
    class: View
    size: fill
    padding: 8dp, 8dp
    subviews: [
      {
        id: number
        class: Label
        size: 40dp, wrap
        align: left, center
        gravityX: right
        font-size: 24dp
      },
      {
        class: ImageView
        id: image
        size: 40dp, 52dp
        align: toRightOf(number 8dp), center
        contentMode: aspectFit
        gravity: center
        background: #333
        padding: 1dp
      },
      {
        id: title
        class: Label
        style: $listview.item-title
        size: fill, wrap
        align: toRightOf(image 8dp), top
      },
      {
        class: Label
        id: subtitle
        style: $listview.item-subtitle
        size: fill, wrap
        align: toRightOf(image 8dp), bottom
      }
    ]
}

}
