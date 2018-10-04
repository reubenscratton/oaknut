{
    class: View
    size: fill
    background: #f8f8f8
    subviews: [
    {
        class: ListView
        id: listView
        size: fill
    },
    {
        class: View
        size: fill, wrap
        background: #ddd
        subviews: [
        {
            class: SearchBox
            id: searchBox
            size: fill, 40dp
        }
        ]
    }
    ]
}
