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



void App::main() {
    //loadStyleAsset("styles.res");

    MainViewController* mainVC = new MainViewController();
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


