{
    class: LinearLayout
    size: fill
    orientation: vertical
    weights: 1,0
    subviews: [
    {
        class: BeebView
        id: beebView
        size: fill, 0
    },
    {
        class: ControllerView
        id: controllerView
        size: fill, aspect(0.675)
        background: #222
    }
    ]
}
