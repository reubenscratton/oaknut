{
    class: LinearLayout
	width: match_parent
	height: match_parent
    background: lightGray
    orientation: vertical
	subviews: [{
        class: View
		id: topBar
		width: match_parent
		height: 44dp
        background: 0xFFDDDDDD
        padding: 4dp
        subviews: [{
            class: LinearLayout
            width: wrap_content
            height: wrap_content
            alignX: left
            alignY: center
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
            }]
        }]
    },{
        class:EditText
		id: editText
		width: match_parent
		height: 0dp
        weight: 1
        background: {
            inset: 4dp
            fill: white
            stroke: darkGray
            stroke-width: 1px
            corner-radii: 8dp
        }
        padding: 16dp
		alignY: below,topBar
	}]
}
}
