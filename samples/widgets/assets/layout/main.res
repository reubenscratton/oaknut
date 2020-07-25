{
class: View
size: fill
subviews: [
    {
    id: drawer
    class: Drawer
    size: 180dp, fill
    background: white
    subviews: [
        {
        id: buttons
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        text: Buttons
        },
        {
        id: checkboxes
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        text: Checkboxes
        },
        {
        id: editboxes
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        text: EditBoxes
        },
        {
        id: lists
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        text: Lists
        },
        {
        id: progressbars
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        text: Progress Bars
        },
        {
        id: radiobuttons
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        text: Radio Buttons
        },
        {
        id: tabs
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        text: Tabs
        }
    ]
    },
    {
    class: View
    size: fill
    alignX: drawer.toRightOf
    subviews: [
        {
        class: Tabs
        style: $material_design.Tabs
        size: fill,wrap
        align: top
        tabs: CONTAINED,OUTLINED,TEXT,EXTRA 1, EXTRA 2, EXTRA 3, EXTRA 4, EXTRA 5
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
]
}
