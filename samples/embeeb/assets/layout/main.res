{
    class: LinearLayout
    width: fill_parent
    height: fill_parent
    orientation: vertical
    weights: 0,1,0
    subviews: [{
        class: BeebView
        id: beebView
        width: fill_parent
        height: aspect(0.8)
      },{
        class: View
        width: fill_parent
        height: 0
        background: black
      },{
        class: ControllerView
        id: controllerView
        width: fill_parent
        height: aspect(0.675)
        background: #222
      }]
}
