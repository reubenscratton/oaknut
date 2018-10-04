{
    class: LinearLayout
    size: fill
    orientation: vertical
    weights: 0,1,0
    subviews: [
    {
        class: BeebView
        id: beebView
        size: fill, aspect(0.8)
    },
    {
        class: View
        size: fill, 0
        background: black
    },
    {
        class: ControllerView
        id: controllerView
        size: fill, aspect(0.675)
        background: #222
    }
    ]
}
