{
class: View
size: fill
background: #333
padding: 16dp
subviews: [
{
    id: instruction
    class: Label
    size: wrap
    align: center, top
    gravityX: center
    padding: 16dp,0
    font-weight: bold
    font-size: 18dp
    forecolor: white
    text: "Press and hold the record button whilst saying the phrase below to capture your voice recording"
},
{
    id: phrase
    class: Label
    size: wrap
    align: center, below(instruction 40dp)
    gravityX: center
    padding: 16dp,0
    font-weight: bold
    font-size: 32dp
    forecolor: #666
    text:"\"My voice is my password and it proves my identity\""
},
{
    id: wav
    class: WavView
    align: left, below(phrase 20dp)
    size: fill, 140dp
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
},
{
    id: recordLabel
    class: Label
    size: wrap
    align: center, below(record 8dp)
    font-weight: light
    font-size: 12dp
    forecolor: white
    text: "Press and hold to record"
},

]
}


