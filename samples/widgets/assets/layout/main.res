{
class: View
size: fill
subviews: [
    {
    class: Tabs
    size: fill,wrap
    align: top
    tabs: CONTAINED,OUTLINED,TEXT
    },
    {
    class: LinearLayout
    size: wrap
    align: center
    spacing: 16dp
    subviews: [
        {
        class: MDButton
        style: $material_design
        text: BUTTON
        },
        {
        class: MDButton
        style: $material_design
        text: BUTTON
        }
    ]
    }
]
}
