{
class: View
size: fill
subviews: [
    {
    id: content
    class: BNContentView
    size: fill
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
        image: images/top_stories@2x.png
        text: Top stories
        tint: $color.bbcNewsRed
    },
    {
        class: BNTabBarButton
        image: images/my_news@2x.png
        text: My news
    },
    {
        class: BNTabBarButton
        image: images/popular@2x.png
        text: Popular
    },
    {
        class: BNTabBarButton
        image: images/video@2x.png
        text: Video
    },
    {
        class: BNTabBarButton
        image: images/search@2x.png
        text: Search
    }
    ]
    }
]
}
