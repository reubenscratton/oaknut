//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

void test_string();

class MainViewController : public ViewController {
public:
    MainViewController() {
    }
};


class SerializableObj : public Object, public ISerializeToVariant {
public:
    int _int;
    string _str;
    float _float;
    
    SerializableObj(int i, string s, float f) {
        _int = i;
        _str = s;
        _float = f;
    }
    
    void fromVariant(const variant& v) override {
        _int = v.intVal("i");
        _str = v.stringVal("s");
        _float = v.floatVal("f");
    }
    void toVariant(variant& v) override {
        v["i"] = _int;
        v["s"] = _str;
        v["f"] = _float;
    }
};

void assert_rects_equal(const RECT& r1, const RECT& r2) {
    assert(r1.origin.x==r2.origin.x);
    assert(r1.origin.y==r2.origin.y);
    assert(r1.size.width==r2.size.width);
    assert(r1.size.height==r2.size.height);
}

class TestApp : public App {
public:
    void main() override {
        //loadStyleAsset("styles.res");

        MainViewController* mainVC = new MainViewController();
        
        RECT constraint = {0,0,100,100};
        
        // Root view
        View* view = new View();
        view->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
        view->layout(constraint);
        assert_rects_equal(view->getRect(), {0,0,100,100});
        mainVC->setView(view);
        
        // Inner view to fill root view with some padding
        View* view2 = new View();
        view2->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Fill());
        view->setPadding({8,8,8,8});
        view->addSubview(view2);
        view->layout(constraint);
        assert_rects_equal(view2->getRect(), {8,8,84,84});

        // Leaf view 1 in top left
        View* viewLeaf1 = new View();
        viewLeaf1->setLayoutSize(MEASURESPEC::Abs(10), MEASURESPEC::Abs(10));
        view2->addSubview(viewLeaf1);
        view->layout(constraint);
        assert_rects_equal(viewLeaf1->getRect(), {0,0,10,10});

        // Leaf view 2 just right of leaf view 1
        View* viewLeaf2 = new View();
        viewLeaf2->setLayoutSize(MEASURESPEC::Abs(10), MEASURESPEC::Abs(10));
        viewLeaf2->setLayoutOrigin(ALIGNSPEC::ToRightOf(viewLeaf1, 2), ALIGNSPEC::Top());
        view2->addSubview(viewLeaf2);
        view->layout(constraint);
        assert_rects_equal(viewLeaf2->getRect(), {12,0,10,10});

        // Leaf view 3 to fill remaining space
        View* viewLeaf3 = new View();
        viewLeaf3->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Abs(10));
        viewLeaf3->setLayoutOrigin(ALIGNSPEC::ToRightOf(viewLeaf2, 2), ALIGNSPEC::Top());
        view2->addSubview(viewLeaf3);
        view->layout(constraint);
        assert_rects_equal(viewLeaf3->getRect(), {24,0,60,10});
        

        // Move leaf 1 to top right corner
        viewLeaf1->setLayoutOrigin(ALIGNSPEC::Right(), ALIGNSPEC::Top());
        view->layout(constraint);
        assert_rects_equal(viewLeaf1->getRect(), {74,0,10,10});

        // Move leaf 2 to be left of leaf 1
        viewLeaf2->setLayoutOrigin(ALIGNSPEC::ToLeftOf(viewLeaf1, 2), ALIGNSPEC::Top());
        view->layout(constraint);
        assert_rects_equal(viewLeaf2->getRect(), {62,0,10,10});

        // Move leaf 3 to fill available space left of leaf 2
        viewLeaf3->setLayoutOrigin(ALIGNSPEC::ToLeftOf(viewLeaf2, 2), ALIGNSPEC::Top());
        view->layout(constraint);
        assert_rects_equal(viewLeaf3->getRect(), {0,0,60,10});

        // Move leaf 1 to center, verify other leafs reposition correctly
        viewLeaf1->setLayoutOrigin(ALIGNSPEC::Center(), ALIGNSPEC::Center());
        view->layout(constraint);
        assert_rects_equal(viewLeaf1->getRect(), {37,37,10,10});
        assert_rects_equal(viewLeaf2->getRect(), {25,0,10,10});
        assert_rects_equal(viewLeaf3->getRect(), {0,0,23,10});

        _window->setRootViewController(mainVC);

        test_string();
        
        
        /*LocalStore* localStore = LocalStore::create("test", "i");
        localStore->open();
        localStore->put(new SerializableObj(1, "foo", 3.14f));
        localStore->put(new SerializableObj(2, "bar", 2.7818f));
        localStore->put(new SerializableObj(3, "baz", 8.444f));
        localStore->close();
        
        localStore->open();
        localStore->moveFirst();
        VariantMap map;
        assert(localStore->readAndAdvance(map));
        assert(map["i"] == 1);
        assert(map["s"] == "foo");
        assert(map["f"] == 3.14f);
        assert(localStore->readAndAdvance(map));
        assert(map["i"] == 2);
        assert(map["s"] == "bar");
        assert(map["f"] == 2.7818f);
        localStore->close();*/

        log("Tests passed!");
        exit(0);
    }
} the_app;


