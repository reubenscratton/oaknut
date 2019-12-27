{
window: {
    default-width:375dp
    default-height:580dp
    status-bar-light: true
    background-color: $color.windowBackground
}

NavigationBar: {
    background: $color.bbcNewsRed
    tint: #FFF
}

color: {
    bbcNewsRed: #CC0000
    bbcNewsLiveRed: #CC0000
    window: #e4e4e4
    windowBackground: #e4e4e4
    contentBackground: white
    contentForeground: #404040
    contentBackgroundInv: #1e1e1e
    contentForegroundInv: white
    topicInv: white
    topic: #CC0000
    thinDivider: #26000000
    thinDividerInv: #26FFFFFF
    timestamp: #999999
    timestampInv: #A0A0A0
    articleSection: #CC352B
    captionText: #848484
    pglCaptionText: white
    defaultSelectedFill: #CCCCCC
    featureMagazine: $color.bbcNewsLiveRed
    featureAnalysis: $color.bbcNewsLiveRed
    featureFeatures: $color.bbcNewsLiveRed
    featureSport: #FFDF43
    cellOverlayBackground: #80000000
}

H1: {
    font-weight: medium
    font-size: 24sp
    font-size@tablet: 40sp
    line-height: 30sp
    line-height@tablet: 48sp
    forecolor: black
    forecolor-inv: white
}

H2: {
    font-weight: medium
    font-size: 15sp
    font-size@tablet: 24sp
    line-height: 17.5sp
    line-height@tablet: 30sp
    forecolor: black
    forecolor-inv: white
}

H3: {
    font-weight: medium
    font-size: 15sp
    font-size@tablet: 20sp
    line-height: 17.5sp
    line-height@tablet: 24sp
    forecolor: black
    forecolor-inv: white
}

featureHeadline: {
    font-weight: medium
    font-size@tablet: 24sp
    font-size: 15sp
    line-height@tablet: 24sp
    line-height: 15sp
    forecolor: black
    forecolor-inv: white
}

featureHeadlineFullWidth: {
    font-weight: medium
    font-size@tablet: 30sp
    font-size: 22sp
    line-height@tablet: 30sp
    line-height: 22sp
    forecolor: black
    forecolor-inv: white
}

font-sizes: {
    timestamp@tablet: 17sp
    timestamp: 14sp
    summary@tablet: 18sp
    summary: 14sp
    summary@4inch: 13sp
    titleModule@tablet: 28sp
    titleModule: 22sp
    ribbon@tablet: 20sp
    ribbon: 16sp
    cornerLabels@tablet: 20sp
    cornerLabels: 24sp
    see-all@tablet: 18sp
    see-all:12sp
}


line-heights: {
    timestamp@tablet: 22sp
    timestamp: 18sp
    summary@tablet: 24sp
    summary: 18sp
    summary@4inch: 17sp
}

cells: {
    top-story: {
        text-padding: 8dp,5dp
    }
}

text-insets: {
    top-story@tablet:32dp,20dp,32dp,32dp
    top-story:8dp,5dp
    secondary-top-story@tablet:16dp,10dp
    secondary-top-story:8dp,5dp,8dp,3dp
    feature@tablet:16dp,10dp,16dp,12dp
    feature:12dp,8dp,12dp,24dp
    related-story@tablet:32dp,16dp,32dp,0
    related-story:16dp,8dp,16dp,0
}

topicAndTimestamp: {
    font-weight: light
    font-size: $font-sizes.timestamp
    line-height: $line-heights.timestamp
    padding: 3dp,5dp
    forecolor: $color.timestamp
    forecolor-inv: $color.timestampInv
    lines: 1
    ellipsize: true
}
tinyTimestamps: {
    font-weight: light
    font-size: 14sp
    color: gray
    line-height: 0
    padding:0
}

summaryText: {
    font-weight: light
    font-size: $font-sizes.summary
    line-height: $line-heights.summary
    forecolor: $color.contentForeground
    forecolor-inv: white
    padding:0dp
}


indexTitle: {
    padding: 16dp,16dp,16dp,0dp
    font-weight: light
    font-size: 17sp
}


relatedTitle: {
    padding: 16dp,16dp,16dp,8dp
    font-weight: light
    font-size: 20sp
    top-divider-color: #DDD
    top-divider-height:4dp
}

relatedTopic: {
    size: fill,wrap
    font-weight: light
    font-size: 18sp
    font-size@tablet: 20sp
    line-height: 0
    forecolor: $color.topic
    forecolor-inv: $color.topicInv
    padding@tablet: 12dp,0
    padding: 16dp,8dp,16dp,0dp
}
linkDivider: {
    size: 100%-32dp, 1dp
    left: 16dp
    background: #FFEEEEEE
}


durationSmall: {
    font-weight: light
    font-size: $font-sizes.summary
    line-height: 0
    padding:8dp,4dp
    forecolor: white
}
durationTiny: {
    font-weight: light
    font-size: $font-sizes.see-all
    line-height: 0
    padding:4dp,2dp
    forecolor: white
}

titleModule: {
    font-weight: light
    font-size: $font-sizes.titleModule
    forecolor: black
}

digestTitles: {
    font-weight: light
    font-size: $font-sizes.titleModule
    forecolor: black
    line-height:0
    padding@tablet: 16dp
    padding: 8dp
}

cornerLabels: {
    size@tablet: 40dp
    size: 44dp
    font-weight: medium
    font-size: $font-sizes.cornerLabels
    forecolor: white
}

ribbon: {
    font-weight: medium
    font-size: $font-sizes.ribbon
    color: white
    height@tablet: 50dp
    height: 44dp
    padding@tablet: 24dp,0
    padding: 16dp,0
    min-left@tablet: 64dp
    min-left: 40dp
    max-left@tablet: 128dp
    max-left: 64dp
}

sideMenu: {
    section: {
        font-size: 18sp
        padding: 16dp,0,0,0
        forecolor: white
    }
    cell: {
        font-weight: light
        font-size: 18sp
        padding: 32dp,0,0,0
        forecolor: white
    }
}

byline: {
    thumbnail-size: 38dp
    font-size: $font-sizes.summary
    forecolor: black
    line-height: $line-heights.summary
    name: {
        font-weight: medium
    }
    function: {
        font-weight: light
    }
}

imageCaption: {
    size: fill, wrap
    background: #f8f8f8
    font-weight: light
    font-size: $font-sizes.summary
    forecolor: #666666
    padding@tablet: 32dp,24dp,32dp,16dp
    padding: 16dp,8dp,16dp,8dp
}

// Top Stories : Photo gallery cell
photoGalleryCell: {
    forecolor: white
    line-height:0
    headline: $H2
    summary: {
        font-weight: light
        font-size: $font-sizes.summary
        padding@tablet:0,4dp,0,8dp
        padding:0,2dp,0,4dp
    }
    cameraIconPadding@tablet:80dp,72dp
    cameraIconPadding:40dp,36dp
}

photoGallery: {
    forecolor: white
    line-height:0
    landingPageHeadline: {
        font-weight: medium
        font-size: $font-sizes.H1
        padding@tablet:32dp,64dp,32dp,0
        padding:16dp,32dp,16dp,0
    }
    landingPageSummary: {
        font-weight: light
        font-size: $font-sizes.summary
        padding@tablet:32dp,8dp,32dp,16dp
        padding:16dp,4dp,16dp,8dp
    }
}

photoGalleryCaptions: {
    font-weight: light
    font-size: $font-sizes.summary
    forecolor: white
    line-height: 0
    padding:16dp,8dp
}

followScreen: {
    titleIPad: {
        font-weight: medium
        font-size: 18sp
        forecolor:bbcNewsLiveRed
        line-height:0
        padding:0
    }
    header: {
        font-weight: medium
        font-size: 18sp
        forecolor: black
        line-height:0
        padding:16dp,8dp,16dp,2dp
    }
    item: {
        font-weight: light
        font-size: 16sp
        forecolor: black
        line-height:0
        padding:0
    }
}

ambientNotification: {
    font-weight: medium
    font-size: 14sp
    forecolor: white
    line-height: 0
    padding:8dp,2dp
}

seeAllButton: {
    font-weight: light
    font-size: $font-weights.see-all
    forecolor:$color.contentForeground
    line-height:0
    padding:0
}

// LEPs
keyPoints: {
    font-weight: light
    font-size: $font-sizes.basic
    line-height: $line-heights.basic
    forecolor: white
    padding: 42dp,0,8dp,0
    title: {
        padding: 16dp,0
    }
}


article: {

    text: {
    
        default: {
            font-size: 18sp
            font-weight: light
            line-height: 24sp
            padding: 16dp,12dp,0dp,12dp
            forecolor: #404040
        }
        introduction: {
            font-weight: medium
            forecolor: black
        }
        transmission-info: {
            font-weight: light
            font-style: italic
        }
        crosshead: {
            font-weight: medium
            font-size: 22sp
        }
        list: {
            font-weight: light
            padding: 0,5dp
        }
        question: {
            font-weight: medium
        }
        answer: {
            font-weight: light
        }
        heading: {
            font-weight: medium
        }
        subheading: {
            font-weight: medium
        }
        commentaryHeading: {
            font-weight: medium
            font-size: $font-sizes.H1
            padding:0,4dp
        }
        commentarySubheading: {
            font-weight: medium
            padding:0,4dp
        }
        commentaryTime: {
            font-weight: medium
            padding:0,4dp
        }
    }


}



copyright: {
    size: fill,wrap
    gravity: center
    font-weight: medium
    font-size: 16sp
    padding:16dp
    forecolor: $color.contentForeground
}

web-browser: {
    title: {
        font-weight: medium
        font-size: $font-sizes.ribbon
    }
    url: {
        font-weight: regular
        font-size: $font-sizes.summary
    }
    color: white
    line-height:0
    padding:0
}


dummyText: {
    font-weight: light
    font-size: 24sp
    color: gray
    line-height:0
    padding@tablet: 16dp
    padding: 8dp
}

redButtonTitles: {
    font-weight: light
    font-size: $font-sizes.timestamp
    color: white
}

advertLabel: {
    font-size: 11dp
    color: #80FFFFFF
}


featureAccentStripHeight: 2dp


// iOS accessibility: these correspond to UIContentSizeCategory
dynamicTextDeltas: {
    extraSmall: -3
    small: -2
    medium: -1
    large: 0
    extraLarge: 2
    extraExtraLarge: 3
    extraExtraExtraLarge: 4
    accessibilityMedium: 5
    accessibilityLarge: 6
    accessibilityExtraLarge: 7
    accessibilityExtraExtraLarge: 8
    accessibilityExtraExtraExtraLarge: 9
}

}
