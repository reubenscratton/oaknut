{
class: View
size: fill
subviews: [
    {
    id: content
    class: BNContentView
    size: fill
    scroll-insets:0,0,0,60dp
    padding-bottom: 68dp
    },
    {
    id: tabbar
    class: TabBar
    size: fill, 60dp
    align: left, bottom
    background: #fff
    subviews: [
    {
        class: BNTabBarButton
        image: images/top_stories.png
        text: Top stories
        tint: $color.bbcNewsRed
    },
    {
        class: BNTabBarButton
        image: images/my_news.png
        text: My news
    },
    {
        class: BNTabBarButton
        image: images/popular.png
        text: Popular
    },
    {
        class: BNTabBarButton
        image: images/video.png
        text: Video
    },
    {
        class: BNTabBarButton
        image: images/search.png
        text: Search
    }
    ]
    }
]
}
