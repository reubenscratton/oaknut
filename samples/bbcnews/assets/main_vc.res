{
class: View
size: fill
subviews: [
    {
    id: content
    size: fill
    subviews: [
        {
        id: top-stories
        class: BNContentView
        size: fill
        },
        {
        id: my-news
        class: BNContentView
        size: fill
        },
        {
        id: popular
        class: BNContentView
        size: fill
        },
        {
        id: video
        class: BNContentView
        size: fill
        },
        {
        id: search
        size: fill
        }
    ]
    },
    {
    id: tabbar
    class: TabBar
    size: fill, wrap
    align: left, bottom
    background: #fff
    button-class: BNTabBarButton
    tint: #444
    selected-tint: $color.bbcNewsRed
    buttons: [
    {
        image: images/top_stories.png
        text: Top stories
    },
    {
        image: images/my_news.png
        text: My news
    },
    {
        image: images/popular.png
        text: Popular
    },
    {
        image: images/video.png
        text: Video
    },
    {
        image: images/search.png
        text: Search
    }
    ]
    }
]
}
