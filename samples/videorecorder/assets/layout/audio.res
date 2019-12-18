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
    font-weight: bold
    font-size: 18dp
    forecolor: white
    text: "Press and hold the record button to record yourself saying this phrase:"
},
{
    id: phrase
    class: Label
    size: wrap
    align: center, bottom+32dp
    gravityX: center
    font-weight: bold
    font-size: 32dp
    forecolor: #666
    text:"\"My voice is my password and it proves my identity\""
},
{
    id: rec1
    class: RecordingView
    label: "1."
    align: left, phrase.bottom+16dp
},
{
    id: rec2
    class: RecordingView
    label: "2."
    align: left, rec1.bottom+16dp
},
{
    id: rec3
    class: RecordingView
    label: "3."
    align: left, rec2.bottom+16dp
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
},
{
    id: recordLabel
    class: Label
    size: wrap
    align: center, record.bottom+8dp
    font-weight: light
    font-size: 12dp
    forecolor: white
    text: "Press and hold to record"
},

]
}


