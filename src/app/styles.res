R"({
font-size: 17dp

App: {
    max-io-threads: 50
    cache: {
        ram: 50MB
        disk: 100MB
    }
}

Window: {
    default-width:200dp
    default-height:200dp
    safeInsetBackgrounds: {
        bottom: {
            @default: transparent
            @android: black
        }
        top:0
    }
    scrim: #80000000
    scrollbars: {
        width: 2.5dp
        color: #777
        corner-radius: 2.5dp
        min-length: 40dp
        inset: 2dp
        fade-in-delay: 500
        fade-duration: 300
        fade-out-delay: 800
    }
    animation-duration: 350
    image-fade-in: {
        threshold: 250
        duration: 500
    }

}

colors: {
    table_bkgnd: #efeff4
    table_border: #c8c7cc
    table_chevron:  #c7c7cc
    table_selection_bkgnd: #d9d9d9
    text_link_default: #007AFF
}

ListView: {
    background: {
        @default: #00000000
        @selected: #d9d9d9
    }
    divider-height: 1px
    divider-color: #ccc
    item-title: {
        forecolor: #333
    }
    item-subtitle: {
        forecolor: #777
    }
}

Label: {
    font-size: $font-size
    forecolor: black
}

Button: {
    size: wrap
    corner-radii: 4dp
    background-color: {
        @default: lightGray
        @pressed: gray
    }
    gravity: center
    padding: 16dp,8dp
    image-spacing: 8dp
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
    background-color: white
    background-inset: 6dp
    stroke: darkGray
    stroke-width: 1px
    corner-radii: 8dp
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
            @default: #F7F6F5
            @pressed: #DDD
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
