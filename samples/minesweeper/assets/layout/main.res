{
    class: View
	width: match_parent
	height: match_parent
	background: white
	subviews: [{
        class: View
		id: topBar
		width: 250dp
		height: 70dp
		alignX: center
		subviews: [{
            class: Label
			id: flags
			width: wrap_content
			height: wrap_content
			alignX: left
			alignY: center
			text:0
        },{
            class: ImageView
			id: face
			alignX: center
			alignY: center
			width: 50dp
			height: 50dp
		},{
            class: Label
			id: remaining
			width: wrap_content
			height: wrap_content
			alignX: right
			alignY: center
			text:0
			gravityX:right
		}]
    },{
        class: GameView
		id: game
		width: wrap_content
		height: wrap_content
		alignX: center
		alignY: below,topBar
	}]
}
