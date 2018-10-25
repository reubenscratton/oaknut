{
    class: View
	size: fill
	background: white
	subviews: [{
        class: View
		id: topBar
		size: 250dp, 70dp
		alignX: center
		subviews: [{
            class: Label
			id: flags
			size: wrap
			align: left, center
			text:0
        },{
            class: ImageView
			id: face
            size: 50dp
            contentMode: aspectFit
            align: center
		},{
            class: Label
			id: remaining
			size: wrap
			align: right,center
			text:0
			gravityX:right
		}]
    },{
        class: GameView
		id: game
		size: wrap
		align: center, below(topBar)
	}]
}
