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
        class: Button
        style: $material_design.Button.contained
        text: BUTTON
        },
        {
        class: Button
        style: $material_design.Button.outlined
        text: BUTTON
        },
        {
        class: Button
        style: $material_design.Button.text
        text: BUTTON
        }
    ]
    }
]
}
