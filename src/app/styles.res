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
    width: 40dp
    height: 40dp
    padding: 8dp
    alpha: 0.5
}

EditText: {
    gravityY: center
}

NavigationBar: {
    size: fill,wrap
    preferredContentHeight: 44dp
    background: white
    padding: 8dp,0
    title: {
        font-size: 17dp
        forecolor: black
    }
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


ActionSheet: {
    corner-radius: 12dp
    scrim: #80000000
    divider-color: #ccc
    OuterGroup: {
        orientation: vertical
        size:fill, wrap
        alignY: bottom
        padding: 16dp
        spacing: 8dp
    }
    Group: {
        orientation: vertical
        size:fill, wrap
    }
    Action: {
        size: fill, 57dp
        alignX: center
        alignY: center
        gravityX: center
        gravityY: center
        font-size: 20dp
        forecolor: #007AFF
        background: {
            pressed: #DDD
            unpressed: #F7F6F5
        }
    }
    Title: {
        height: 46dp
        font-size: 18dp
        forecolor: #BBB
    }
    Cancel: {
        font-weight: medium
    }

}


})"
