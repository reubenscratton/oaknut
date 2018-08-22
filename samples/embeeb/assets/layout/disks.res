{
    class: View
    width: fill_parent
    height: fill_parent
    background: #f8f8f8
    subviews: {
        class: ListView
        id: listView
        width: fill_parent
        height: fill_parent
      }, {
        class: View
        width: fill_parent
        height: wrap_content
        background: #ddd
        subviews: [{
            class: SearchBox
            id: searchBox
            width: fill_parent
            height: 40dp
        }]
    }
}
