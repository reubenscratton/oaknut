R"({
font-size: 17dp

window: {
    default-width:200dp
    default-height:200dp
    safeInsetBackgrounds: {
        bottom@android: black
        top:0
    }
    scrim: #80000000
    scrollbars: {
        color: #777
        corner-radius: 2.5dp
    }
}

colors: {
    table_bkgnd: #efeff4
    table_border: #c8c7cc
    table_chevron:  #c7c7cc
    table_selection_bkgnd: #d9d9d9
    text_link_default: #007AFF
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
    gravity: center
}

ToolbarButton: {
    width: 40dp
    height: 40dp
    padding: 8dp
    contentMode: aspectFit
    gravity: center
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
    lines: 1
    gravity: left, center
    padding: 16dp, 2dp, 16dp, 2dp
    forecolor: #000
    iconTint: #555
}

SegmentedControl: {
    font-size: $font-size
    stroke-width: 1dp
    corner-radius: 4dp
    spacing: 1dp
    padding: 8dp,4dp
}


ActionSheet: {
    corner-radius: 12dp
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
        align: center
        gravity: center
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

TextField: {
    height: wrap
    label: {
        size: wrap
        font-size: 12dp
        font-weight: light
    }
    edittext: {
        padding: 8dp, 4dp
        size: fill, wrap
        align: left, below+4dp
    }
}


})"_S
