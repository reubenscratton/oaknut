{
LinearLayout: {
    width: fill_parent
    height: fill_parent
    orientation: vertical
    BeebView: {
        id: beebView
        width: fill_parent
        height: aspect(0.8)
    }
    View: {
        width: fill_parent
        height: 0
        weight: 1
        background: 0xFF000000
    }
    ControllerView: {
        id: controllerView
        width: fill_parent
        height: aspect(0.675)
        background: 0xFF222222
    }
}

}
