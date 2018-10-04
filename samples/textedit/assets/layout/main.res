{
    class: LinearLayout
	size: fill
    background: lightGray
    orientation: vertical
    weights: 0,1
	subviews: [{
        class: View
		id: topBar
		size: fill, 44dp
        background: #ddd
        padding: 4dp
        subviews: [{
            class: LinearLayout
            size: wrap
            align: left, center
            orientation: horizontal
            subviews: [{
                class: ImageView
                id: alignLeft
                style: $barButton
                image: images/align_left.png
            },{
                class: ImageView
                id: alignCenter
                style: $barButton
                image: images/align_center.png
            },{
                class: ImageView
                id: alignRight
                style: $barButton
                image: images/align_right.png
            },{
                class: ImageView
                id: bold
                style: $barButton
                image: images/bold.png
            }]
        }]
    },
    {
        class:EditText
		id: editText
		size: fill, 0dp
        background: {
            inset: 4dp
            fill: white
            stroke: darkGray
            stroke-width: 1px
            corner-radii: 8dp
        }
        padding: 16dp
		alignY: below(topBar)
	}]
}
