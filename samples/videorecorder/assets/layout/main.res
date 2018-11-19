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
hole-shape: oval
hole-size: 66%, aspect(1.333)
hole-align: center
hole-stroke-color: white
hole-stroke-width: 2dp
},
{
id: salutation
class: Label
size: wrap
align: center, top(16dp)
text: Press and hold the button to record
font-size: 15dp
forecolor: white
},
{
id: instruction
class: Label
size: wrap
align: center, below(salutation 8dp)
gravityX: center
padding: 16dp,0
font-weight: bold
font-size: 20dp
forecolor: white
},
{
    id: phrase
    class: Label
    size: wrap
    align: center, below(instruction 40dp)
    gravityX: center
    padding: 16dp,0
    font-weight: bold
    font-size: 34dp
    forecolor: #0096FF
    visibility: hidden
},
{
    id: record
    class: ImageView
    size: 72dp
    align: center, bottom(-16dp)
    image: {
    enabled: images/record_enabled.png
    disabled: images/record_disabled.png
    }
}
]
}


