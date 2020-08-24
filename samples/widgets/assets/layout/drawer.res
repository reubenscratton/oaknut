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
    style: $drawer-button
    text: Buttons
    },
    {
    id: checkboxes
    class: Button
    style: $drawer-button
    text: Checkboxes
    },
    {
    id: editboxes
    class: Button
    style: $drawer-button
    text: EditBoxes
    },
    {
    id: lists
    class: Button
    style: $drawer-button
    text: Lists
    },
    {
    id: progressbars
    class: Button
    style: $drawer-button
    text: Progress Bars
    },
    {
    id: radiobuttons
    class: Button
    style: $drawer-button
    text: Radio Buttons
    },
    {
    id: tabs
    class: Button
    style: $drawer-button
    text: Tabs
    }
]
}
