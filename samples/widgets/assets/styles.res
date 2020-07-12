{
    font-size: 14dp
    Window: {
        default-width:500dp
        default-height:348dp
        background-color: #f6f6f6
    }

    primary_color:#6221ea
  
    material_design: {
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
                    @pressed: $primary_color
                }
                fill-alpha: {
                    @default: 0
                    @pressed: 0.2
                }
                ink-color: $primary_color
                ink-color-alpha: 0.2
                forecolor: $primary_color
            }
            text: {
                corner-radius: 4dp
                fill-color: {
                    @default: transparent
                    @pressed: $primary_color
                }
                fill-alpha: {
                    @default: 0
                    @pressed: 0.2
                }
                ink-color: $primary_color
                ink-color-alpha: 0.2
                forecolor: $primary_color
            }
        }
        Tabs: {
            Button: {
                corner-radius: 0dp
                fill-color: {
                    @default: transparent
                    @pressed: $primary_color
                }
                fill-alpha: {
                    @default: 0
                    @pressed: 0.2
                }
                ink-color: $primary_color
                ink-color-alpha: 0.2
                forecolor: {
                    @default: #666
                    @selected: $primary_color
                }
            }
        }

    }


}
