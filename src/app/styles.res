R"({
font-size: 17dp

window: {
    default-width:200dp
    default-height:200dp
}

colors: {
    table_bkgnd: 0xffefeff4
    table_border: 0xffc8c7cc
    table_chevron:  0xffc7c7cc
    table_selection_bkgnd: 0xffd9d9d9
    text_link_default: 0xff007AFF
}

listview: {
    selected-bkgnd-color: #d9d9d9
    divider-color: #ccc
    item-title: {
        forecolor: #333
    }
    item-subtitle: {
        forecolor: #777
    }
}

ListView: {
    divider-height: 1px
    divider-color: $listview.selected-bkgnd-color
}

Label: {
    font-size: $font-size
    forecolor: black
}

Button: {
    background: {
        enabled: {
            fill: lightGray
            corner-radii: 4dp
        }
        pressed: {
            fill: gray
            corner-radii: 4dp
        }
    }
    gravityX: center
    gravityY: center
}

ToolbarButton: {
    width: aspect(1)
    height: match_parent
    padding: 8dp
    alpha: 0.5
}

EditText: {
    gravityY: center
}

SearchBox: {
    background: {
        inset: 6dp
        fill: white
        stroke: darkGray
        stroke-width: 1px
        corner-radii: 8dp
    }
    maxLines: 1
    gravityX: left
    gravityY: center
    padding: 16dp, 2dp, 16dp, 2dp
    forecolor: #000
}

SegmentedControl: {
    font-size: $font-size
    stroke-width: 1dp
    corner-radius: 4dp
}


navbar: {
    height: 44dp
    background: 0xfff6f6f9
    title: {
        font-size: 17dp
        forecolor: 0xff000000
    }
    bottom-divider-color: 0xffa7a7aa
}


}
)"
