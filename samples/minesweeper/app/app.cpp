//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "app.h"
#include "minesweepergame.h"
#include "minesweeperview.h"



class MainViewController : public ViewController {
public:

    MinesweeperGame* _game;

    MainViewController() {
        _game = new MinesweeperGame(10, 10);

        View *view = new View();
        view->setMeasureSpecs(MEASURESPEC_FillParent, MEASURESPEC_FillParent);
        view->setBackgroundColour(0xFFFFf0f0);
        setView(view);

        MinesweeperView* minesweeperView = new MinesweeperView(_game);
        minesweeperView->setMeasureSpecs(MEASURESPEC_WrapContent, MEASURESPEC_WrapContent);
        view->addSubview(minesweeperView);
    }

};

void oakMain() {
    MainViewController* mainVC = new MainViewController();
    mainWindow->setRootViewController(mainVC);
}


