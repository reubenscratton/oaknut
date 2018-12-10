//
//  emBeeb
//
//  Copyright © 2016-2018 Sandcastle Software Ltd. All rights reserved.
//

#include "app.h"
#include "mainviewcontroller.h"
#include "disksviewcontroller.h"
#include "snapshotsviewcontroller.h"


void App::main() {
	NavigationController* rootVC = new NavigationController();
	rootVC->setTitle("emBeeb");
    rootVC->pushViewController(new MainViewController());
    _window->setRootViewController(rootVC);

    /*
    vector<pair<const string&,const Variant&>> foo; // 24 bytes
    sp<ByteBuffer> foo2;
    string foo3;
    uint32_t s1 = sizeof(foo3);
    
    
    Variant vm1({
        {"name1", 10},
        {"name2", 20},
        {"stringVal","flibbertiflooberts"},
        {"mapVal", Variant({
            {"name3", "froo"}
        })}
    });
    
    ByteBufferStream bb;
    vm1.writeSelfToStream(&bb);
    
    Variant vm2;
    vm2.readSelfFromStream(&bb);
    */
    
    //DisksViewController* mainVC = new DisksViewController(NULL);
    //SnapshotsViewController* mainVC = new SnapshotsViewController(NULL, NULL, NULL, NULL);
}



/*
static void Libbeeb_loadDisc(int length, std::string obj) {
    
    Beeb_postKeyboardEvent(g_hbeeb, 0x100, true);
    shiftHeldForDiscBoot = 400;
    Beeb_bbcLoadDisc(g_hbeeb, (uint8_t*)obj.data(), length, 1);

}
*/
