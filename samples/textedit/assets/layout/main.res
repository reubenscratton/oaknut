{
LinearLayout: {
	width: match_parent
	height: match_parent
    background: lightGray
    orientation: vertical
	View: {
		id: topBar
		width: match_parent
		height: 44dp
        background: 0xFFDDDDDD
        padding: 4dp
        LinearLayout: {
            width: wrap_content
            height: wrap_content
            alignX: left
            alignY: center
            orientation: horizontal
            ImageView: {
                id: alignLeft
                style: $barButton
                image: images/align_left.png
            }
            ImageView: {
                id: alignCenter
                style: $barButton
                image: images/align_center.png
            }
            ImageView: {
                id: alignRight
                style: $barButton
                image: images/align_right.png
            }
        }
	}
	EditText: {
		id: editText
		width: match_parent
		height: 0dp
        weight: 1
        background: {
            inset: 8dp
            fill: white
            stroke: red
            stroke-width: 1
            corner-radii: 8dp
        }
        padding: 16dp
		alignY: below,topBar
	}
	}
}
