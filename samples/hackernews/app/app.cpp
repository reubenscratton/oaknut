//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

struct Story {
    int64_t id;
    int64_t time;
    string title;
    string url;
    string by;
    bool loaded;
    bool errord;
};

class StoryItemView : public View {
public:
    StoryItemView() {
        app.layoutInflateExistingView(this, "layout/story.res");
        _titleLabel = (Label*)findViewById("title");
    }
    void setStory(Story& story) {
        _story = &story;
        if (_req) {
            _req->cancel();
            _req = NULL;
        }
        showStory();
        if (!story.loaded) {
            _req = URLRequest::get(string::format("https://hacker-news.firebaseio.com/v0/item/%ld.json", story.id));
            _req->handleJson([&](URLRequest* req, const variant& json) {
                if (req->error()) {
                    story.errord = true;
                } else {
                    story.errord = false;
                    story.title = json.stringVal("title");
                    story.url = json.stringVal("url");
                    story.by = json.stringVal("by");
                    story.loaded = true;
                }
                showStory();
                _req = NULL;
            });
        }
    }
    void showStory() {
        _titleLabel->setText(_story->loaded ? _story->title : "Loading...");
    }
    
    void detachFromWindow() override {
        View::detachFromWindow();
        if (_req) {
            _req->cancel();
            _req = NULL;
        }
    }
    
protected:
    Label* _titleLabel;
    URLRequest* _req;
    Story* _story;
};

class StoriesAdapter : public SimpleListAdapter<Story, StoryItemView> {
public:
    URLRequest* _req;
    
    StoriesAdapter() : SimpleListAdapter() {
        _req = URLRequest::get("https://hacker-news.firebaseio.com/v0/topstories.json");
        _req->handleJson([&](URLRequest* req, const variant& json) {
            _items.clear();
            auto vecStories = json.arrayVal();
            for (auto storyId : vecStories) {
                Story story;
                story.id = storyId.intVal();
                _items.push_back(story);
            }
            _adapterView->reload();
        });
    }
    
    void bindItemView(View* itemview, LISTINDEX index) override {
        StoryItemView* storyItemView = (StoryItemView*)itemview;
        Story& item = _items[listIndexToRealIndex(index)];
        storyItemView->setStory(item);
    }

    
};

class MainViewController : public ViewController {
public:

    ListView* _listView;


    MainViewController() {
        //View* view = app.layoutInflate("layout/main.res");
        _listView = new ListView();
        setView(_listView);
        _listView->setAdapter(new StoriesAdapter());
    }



};




void App::main() {
    
    loadStyleAsset("styles.res");

    MainViewController* mainVC = new MainViewController();
    _window->setRootViewController(mainVC);
}


