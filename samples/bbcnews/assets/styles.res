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
    bbcNewsRed: #C20026
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
    font-name: Helvetica Neue
    font-weight: medium
    font-size: $font-sizes.H1
    line-height: $line-heights.H1
    forecolor: black
    forecolor-inv: white
}

H2: {
    font-name: Helvetica Neue
    font-weight: medium
    font-size: $font-sizes.H2
    line-height: $line-heights.H2
    forecolor: black
    forecolor-inv: white
}

H3: {
    font-name: Helvetica Neue
    font-weight: medium
    font-size: $font-sizes.H3
    line-height: $line-heights.H3
    forecolor: black
    forecolor-inv: white
}

featureHeadline: {
    font-weight: medium
    font-size~tablet: 24sp
    font-size~!tablet: 15sp
    lineHeight~tablet: 24sp
    lineHeight~!tablet: 15sp
    forecolor: black
    forecolor-inv: white
}

featureHeadlineFullWidth: {
    font-weight: medium
    font-size~tablet: 30sp
    font-size~!tablet: 22sp
    lineHeight~tablet: 30sp
    lineHeight~!tablet: 22sp
    forecolor: black
    forecolor-inv: white
}

font-sizes: {
    H1~tablet: 40sp
    H1: 24sp
    H2~tablet: 24sp
    H2: 15sp
    H3~tablet: 20sp
    H3: 15sp
    timestamp~tablet: 17sp
    timestamp: 14sp
    summary~tablet: 18sp
    summary: 14sp
    summary~4inch: 13sp
    relatedTopic~tablet: 20sp
    relatedTopic: 18sp
    titleModule~tablet: 28sp
    titleModule: 22sp
    ribbon~tablet: 20sp
    ribbon~!tablet: 16sp
    cornerLabels~tablet: 20sp
    cornerLabels~!tablet: 24sp
    see-all~tablet: 18sp
    see-all~!tablet:12sp
    basic~tablet: 20sp
    basic~!tablet: 18sp
    crosshead~tablet: 24sp
    crosshead~!tablet: 22sp
}


line-heights: {
    H1~tablet: 48sp
    H1~!tablet: 30sp
    H2~tablet: 30sp
    H2~!tablet: 17.5sp
    H3~tablet: 24sp
    H3~!tablet: 17.5sp
    timestamp~tablet: 22sp
    timestamp~!tablet: 18sp
    summary~tablet: 24sp
    summary~!tablet: 18sp
    summary~4inch: 17sp
    basic~tablet: 26sp
    basic~!tablet: 24sp
}

cells: {
    top-story: {
        text-padding: 8dp,5dp
    }
}

text-insets: {
    top-story~tablet:32dp,20dp,32dp,32dp
    top-story:8dp,5dp
    secondary-top-story~tablet:16dp,10dp
    secondary-top-story:8dp,5dp,8dp,3dp
    feature~tablet:16dp,10dp,16dp,12dp
    feature:12dp,8dp,12dp,24dp
    related-story~tablet:32dp,16dp,32dp,0
    related-story:16dp,8dp,16dp,0
}

topicAndTimestamp: {
    font-weight: light
    font-size: $font-sizes.timestamp
    line-height: $line-heights.timestamp
    insets: 3dp,5dp
    forecolor: $color.timestamp
    forecolor-inv: $color.timestampInv
    lines: 1
}
tinyTimestamps: {
    font-weight: light
    font-size: 14sp
    color: gray
    lineHeight: 0
    insets:0
}

summaryText: {
    font-weight: light
    font-size: $font-sizes.summary
    line-height: $line-heights.summary
    forecolor: $color.contentForeground
    forecolor-inv: white
    insets:0dp
}


relatedTopic: {
    font-weight: light
    font-size: $font-sizes.relatedTopic
    line-height: 0
    forecolor: $color.topic
    forecolor-inv: $color.topicInv
    insets~tablet: 12dp,0
    insets~!tablet: 8dp,0,6dp,0
}

durationSmall: {
    font-weight: light
    font-size: $font-sizes.summary
    line-height: 0
    insets:8dp,4dp
    color: white
}
durationTiny: {
    font-weight: light
    font-size: $font-sizes.see-all
    line-height: 0
    insets:4dp,2dp
    color: white
}

titleModule: {
    font-weight: light
    font-size: $font-sizes.titleModule
    color: black
}

digestTitles: {
    font-weight: light
    font-size: $font-sizes.titleModule
    color: black
    line-height:0
    insets~tablet: 16dp
    insets~!tablet: 8dp
}

cornerLabels: {
    size~tablet: 40dp
    size~!tablet: 44dp
    font-weight: medium
    font-size: $font-sizes.cornerLabels
    color: white
}

ribbon: {
    font-weight: medium
    font-size: $font-sizes.ribbon
    color: white
    height~tablet: 50dp
    height~!tablet: 44dp
    insets~tablet: 24dp,0
    insets~!tablet: 16dp,0
    min-left~tablet: 64dp
    min-left~!tablet: 40dp
    max-left~tablet: 128dp
    max-left~!tablet: 64dp
}

sideMenu: {
    section: {
        font-size: 18sp
        insets: 16dp,0,0,0
        color: white
    }
    cell: {
        font-weight: light
        font-size: 18sp
        insets: 32dp,0,0,0
        color: white
    }
}

byline: {
    thumbnail-size: 38dp
    font-size: $font-sizes.summary
    color: black
    line-height: $line-heights.summary
    name: {
        font-weight: medium
    }
    function: {
        font-weight: light
    }
}

imageCaptions: {
    font-weight: light
    font-size: $font-sizes.summary
    color: #666666
    insets~tablet: 32dp,24dp,32dp,16dp
    insets~!tablet: 16dp,12dp,16dp,8dp
}

// Top Stories : Photo gallery cell
photoGalleryCell: {
    color: white
    line-height:0
    headline: {
        font-weight: medium
        font-size: $font-sizes.H2
        insets~tablet:0,8dp,0,16dp
        insets~!tablet:0,4dp,0,8dp
    }
    summary: {
        font-weight: light
        font-size: $font-sizes.summary
        insets~tablet:0,4dp,0,8dp
        insets~!tablet:0,2dp,0,4dp
    }
    cameraIconPadding~tablet:80dp,72dp
    cameraIconPadding~!tablet:40dp,36dp
}

photoGallery: {
    color: white
    lineHeight:0
    landingPageHeadline: {
        font-weight: medium
        font-size: $font-sizes.H1
        insets~tablet:32dp,64dp,32dp,0
        insets~!tablet:16dp,32dp,16dp,0
    }
    landingPageSummary: {
        font-weight: light
        font-size: $font-sizes.summary
        insets~tablet:32dp,8dp,32dp,16dp
        insets~!tablet:16dp,4dp,16dp,8dp
    }
}

photoGalleryCaptions: {
    font-weight: light
    font-size: $font-sizes.summary
    color: white
    line-height: 0
    insets:16dp,8dp
}

followScreen: {
    titleIPad: {
        font-weight: medium
        font-size: 18sp
        color:bbcNewsLiveRed
        lineHeight:0
        insets:0
    }
    header: {
        font-weight: medium
        font-size: 18sp
        color: black
        lineHeight:0
        insets:16dp,8dp,16dp,2dp
    }
    item: {
        font-weight: light
        font-size: 16sp
        color: black
        lineHeight:0
        insets:0
    }
}

ambientNotification: {
    font-weight: medium
    font-size: 14sp
    color: white
    line-height: 0
    insets:8dp,2dp
}

seeAllButton: {
    font-weight: light
    font-size: $font-weights.see-all
    color:$color.contentForeground
    lineHeight:0
    insets:0
}

// LEPs
keyPoints: {
    font-weight: light
    font-size: $font-sizes.basic
    line-height: $line-heights.basic
    color: white
    insets: 42dp,0,8dp,0
    title: {
        insets: 16dp,0
    }
}


articles: {
    font-size: $font-sizes.basic
    line-height: $line-heights.basic
    insets: 0,16dp

    normal: {
        font-weight: light
    }
    introduction: {
        font-weight: medium
    }
    transmission-info: {
        font-weight: light
        font-style: italic
    }
    crosshead: {
        font-weight: medium
        font-size: $font-sizes.crosshead
    }
    list: {
        font-weight: light
        insets: 0,5dp
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
        insets:0,4dp
    }
    commentarySubheading: {
        font-weight: medium
        insets:0,4dp
    }
    commentaryTime: {
        font-weight: medium
        insets:0,4dp
    }

}

sportPromo: {
    font-size: 16sp
    insets: 12dp,0
    blurbLabel: {
        font-weight: regular
        color: white
    }
    actionLabel: {
        font-weight: medium
        color: black
        gravity: center
    }
}
imageLink: $sportPromo


copyrightNotice: {
    font-weight: medium
    font-size: 16sp
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
    lineHeight:0
    insets:0
}

elections: {
    title: {
        font-weight: medium
        font-size: 16sp
        color: black
        insets~tablet: 16dp,0
        insets~!tablet: 8dp,0
    }
    link: {
        font-weight: light
        font-size: $font-sizes.relatedTopic
        color:black
        insets~tablet: 16dp
        insets~!tablet: 8dp
    }
    link-red: {
        font-weight: light
        font-size: $font-sizes.relatedTopic
        color: $color.bbcNewsLiveRed
        insets~tablet: 16dp,8dp
        insets~!tablet: 8dp,4dp
    }
    line-height: 0
}


dummyText: {
    font-weight: light
    font-size: 24sp
    color: gray
    line-height:0
    insets~tablet: 16dp
    insets~!tablet: 8dp
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
