//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>


class MyAdapter : public SimpleListAdapter<string> {
public:
    MyAdapter(const vector<string>& vec) {
        for (auto& it : vec) {
            _items.push_back(it);
        }
    }
    View* createItemView(LISTINDEX index) override {
        Label* label = new Label();
        label->setPadding(EDGEINSETS(16, 8, 16, 8));
        label->setGravity({GRAVITY_LEFT, GRAVITY_CENTER});
        return label;
    }
    void bindItemView(View* itemview, LISTINDEX index) override {
        auto item = getItem(index);
        ((Label*)itemview)->setText(item);
    }

};

class ListViewApp : public App {
    void main() override {
        View* rootView = new View();
        rootView->setBackgroundColor(0xFFFFFFFF);
        rootView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
        //rootView->setClipsContents(false);
        const int COLS=500;
        const int ROWS=20;
        for (int x=0 ; x<COLS ; x++) {
            for (int y=0 ; y<ROWS ; y++) {
                Label* label = new Label();
                label->setLayoutOrigin(ALIGNSPEC::Abs(x*400), ALIGNSPEC::Abs(y*180));
                label->setLayoutSize(MEASURESPEC::Abs(400), MEASURESPEC::Abs(180));
                label->setPadding(EDGEINSETS(16, 16, 16, 8));
                label->setGravity({GRAVITY_LEFT, GRAVITY_CENTER});
                label->setText(string::format("Item %d,%d", x, y));
                label->setBackgroundColor(0xFFFF0000 + (((uint32_t)(y*(255/(float)ROWS)))<<8) + ((uint32_t)(x*(255/(float)COLS))));
                View* v = new View();
                v->setLayoutSize(MEASURESPEC::Abs(24), MEASURESPEC::Abs(24));
                v->setLayoutOrigin(ALIGNSPEC::Right(), ALIGNSPEC::Center());
                v->setBackgroundColor(0xFFFF00FF);
                label->addSubview(v);
                rootView->addSubview(label);
            }
        }
        ViewController* vc = new ViewController();
        vc->setView(rootView);
        _window->setRootViewController(vc);
        /*
        ListView* listView = new ListView();
        listView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
        listView->setBackgroundColor(0xFFFFFFFF);
        listView->setAdapter(new MyAdapter(items));
        ViewController* vc = new ViewController();
        vc->setView(listView);
        _window->setRootViewController(vc);
         */
    }
} the_app;
