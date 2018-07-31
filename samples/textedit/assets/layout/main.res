{
LinearLayout: {
	width: match_parent
	height: match_parent
    orientation: vertical
	View: {
		id: topBar
		width: match_parent
		height: 44dp
        background: 0xFFDDDDDD
	}
	EditText: {
		id: editText
		width: match_parent
		height: 0dp
        weight: 1
        background: 0xFFFFFFFF
        padding: 16dp
		alignY: below(topBar)
	}
	}
}
