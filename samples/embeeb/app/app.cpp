//
//  emBeeb
//
//  Copyright © 2016-2018 Sandcastle Software Ltd. All rights reserved.
//

#include "app.h"
#include "ux.main/mainviewcontroller.h"
#include "ux.games/gamesviewcontroller.h"
#include "ux.snapshots/snapshotsviewcontroller.h"


void App::main() {
	NavigationController* rootVC = new NavigationController();
	rootVC->setTitle("emBeeb");
    rootVC->pushViewController(new MainViewController());
    _window->setRootViewController(rootVC);

    //GamesViewController* mainVC = new GamesViewController(NULL);
    //SnapshotsViewController* mainVC = new SnapshotsViewController(NULL, NULL, NULL, NULL);
}



/*
static void Libbeeb_loadDisc(int length, std::string obj) {
    
    Beeb_postKeyboardEvent(g_hbeeb, 0x100, true);
    shiftHeldForDiscBoot = 400;
    Beeb_bbcLoadDisc(g_hbeeb, (uint8_t*)obj.data(), length, 1);

}
*/
