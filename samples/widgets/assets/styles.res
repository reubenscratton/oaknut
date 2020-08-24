{
    font-size: 14dp
    Window: {
        default-width:500dp
        default-height:348dp
        background-color: #f6f6f6
    }

    drawer-button: {
        class: Button
        style: $material_design.Tabs.Button
        width: fill
        gravityX: left
        selectable: true
    }

    primary_color:#6221ea
  
  // TODO: need a way to declare colours that are different alphas of other colours
    primary_color_vfaint:#106221ea
    primary_color_faint:#206221ea
    primary_color_ink:#306221ea

    material_design: {
        Checkbox: {
            padding: 8dp
            size: 34dp
            corner-radius: 16dp
            fill-color: {
                @default: transparent
                @pressed: $primary_color_faint
                @hover: $primary_color_vfaint
            }
            ink-color: $primary_color_ink
            box-color: $primary_color
            box-corner-radius: 2dp
            tick-color: white
        }
        Button: {
            contained: {
                corner-radius: 4dp
                background-color: $primary_color
                forecolor: white
                elevation: {
                    @default: 2dp
                    @pressed: 8dp
                }
                ink-color: #60FFFFFF
            }
            outlined: {
                corner-radius: 4dp
                stroke-color: $primary_color
                stroke-width: 1dp
                fill-color: {
                    @default: transparent
                    @pressed: $primary_color_faint
                    @hover: $primary_color_vfaint
                }
                ink-color: $primary_color_ink
                forecolor: $primary_color
            }
            text: {
                corner-radius: 4dp
                fill-color: {
                    @default: transparent
                    @pressed: $primary_color_faint
                    @hover: $primary_color_vfaint
                }
                ink-color: $primary_color_ink
                forecolor: $primary_color
            }
        }
        Tabs: {
            bar-color: $primary_color
            bar-height: 2dp
            Button: {
                corner-radius: 0dp
                padding: 16dp, 12dp
                fill-color: {
                    @default: transparent
                    @hover: $primary_color_vfaint
                    @pressed: $primary_color_faint
                    @selected: $primary_color_faint
                }
                ink-color: $primary_color_ink
                forecolor: {
                    @default: #666
                    @selected: $primary_color
                }
                icon-tint: true
            }
        }

    }


}
