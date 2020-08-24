{
class: View
size: fill
subviews: [
    layout/drawer.res,
    {
    id: content
    class: ViewSwitcher
    size: fill
    alignX: drawer.toRightOf
    subviews: [
        layout/buttons.res,
        layout/tabs.res
    ]
    }
]
}
