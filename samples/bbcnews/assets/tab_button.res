{
    size: 20%, 60dp
    padding: 8dp
    gravity: center
    tint: #444
    subviews: [
        {
            id: label
            class: Label
            size:  wrap
            align: center, bottom
            font-size: 8dp
        },
        {
            id: image
            class: ImageView
            size:  wrap
            align: center, label.above-4dp
        }
    ]
}
