{
    class: LinearLayout
    width: fill_parent
    height: fill_parent
    orientation: vertical
    subviews: [{
        class: BeebView
        id: beebView
        width: fill_parent
        height: aspect(0.8)
      },{
        class: View
        width: fill_parent
        height: 0
        weight: 1
        background: 0xFF000000
      },{
        class: ControllerView
        id: controllerView
        width: fill_parent
        height: aspect(0.675)
        background: 0xFF222222
      }]
}
