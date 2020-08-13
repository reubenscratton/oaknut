{
class: View
size: fill
subviews: [
    {
    id: drawer
    class: Drawer
    size: 180dp, fill
    elevation: {
        @default: 0dp
        @mobile: 12dp
    }
    background: white
    subviews: [
        {
        class: Label
        style: $material_design.Tabs.Button
        size: fill,wrap
        font-size:24dp
        selectable: false
        text: Widgets
        },
        {
        id: buttons
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        selectable: true
        text: Buttons
        },
        {
        id: checkboxes
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        selectable: true
        text: Checkboxes
        },
        {
        id: editboxes
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        selectable: true
        text: EditBoxes
        },
        {
        id: lists
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        selectable: true
        text: Lists
        },
        {
        id: progressbars
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        selectable: true
        text: Progress Bars
        },
        {
        id: radiobuttons
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        selectable: true
        text: Radio Buttons
        },
        {
        id: tabs
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        selectable: true
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
        orientation: vertical
        size: wrap
        align: center
        spacing: 16dp
        subviews: [
            {
            class: Checkbox
            style: $material_design.Checkbox
            },
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
