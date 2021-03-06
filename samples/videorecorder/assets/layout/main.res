{
class: View
size: fill
subviews: [
{
    id: cameraView
    class: CameraView
    size: fill
    background: black
    contentMode:aspectFill
    gravity:center
},
{
    id: maskView
    class: MaskView
    size: fill
    background: #80000000
    hole-shape: rect
    hole-size: 66%, aspect*1.586
    hole-align: center
    hole-stroke-width: 2dp
    hole-corner-radius: 8dp
},
{
    id: salutation
    class: Label
    size: wrap
    align: center, top+16dp
    text: Press and hold the button to record
    font-size: 15dp
    forecolor: white
},
{
    id: instruction
    class: Label
    size: wrap
    align: center, salutation.bottom+8dp
    gravityX: center
    padding: 16dp,0
    font-weight: bold
    font-size: 18dp
    forecolor: white
},
{
    id: phrase
    class: Label
    size: wrap
    align: center, instruction.bottom+40dp
    gravityX: center
    padding: 16dp,0
    font-weight: bold
    font-size: 32dp
    forecolor: #0096FF
    visibility: hidden
},
{
    id: record
    class: ImageView
    size: 72dp
    align: center, bottom-16dp
    image: {
    enabled: images/record_enabled.png
    disabled: images/record_disabled.png
    }
}
]
}


