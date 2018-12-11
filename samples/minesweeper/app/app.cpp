//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>
#include "game.h"
#include "gameview.h"


class MainViewController : public ViewController, Game::Callback {
public:

    Game* _game;
    GameView* _gameView;
    Label* _labelNumFlags;
    ImageView* _imageViewFace;
    Label* _labelNumRemaining;

    MainViewController() {

        View* view = app.layoutInflate("layout/main.res");
        _gameView = (GameView*)view->findViewById("game");
        _labelNumFlags = (Label*)view->findViewById("flags");
        _imageViewFace = (ImageView*)view->findViewById("face");
        _imageViewFace->onInputEvent = [=](View* view, INPUTEVENT* event) -> bool {
            if (event->type == INPUT_EVENT_TAP) {
                if (_game->_state != InProgress) {
                    _game->restart();
                }
            }
            return true;
        };
        _labelNumRemaining = (Label*)view->findViewById("remaining");
        setView(view);

        _game = new Game(this, 10, 10);
        _gameView->setGame(_game);
        _game->restart();
    }

    // Game callbacks
    virtual void onGameStateChanged() {
        switch (_game->_state) {
            case InProgress:
                _imageViewFace->setImageAsset("images/face_normal.png");
                break;
            case Won:
                _imageViewFace->setImageAsset("images/face_won.png");
                break;
            case Lost:
                _imageViewFace->setImageAsset("images/face_lost.png");
                break;
        }
    }
    virtual void onGameNumFlagsChanged() {
        _labelNumFlags->setText(string::format("Mines: %d", _game->_flags));
    }
    virtual void onGameNumRemainingChanged() {
        _labelNumRemaining->setText(string::format("Closed: %d", _game->_unknown));
    }
    virtual void onCellStateChanged(Cell& cell) {
        _gameView->updateCell(cell);
    }

};

void App::main() {
    MainViewController* mainVC = new MainViewController();
    _window->setRootViewController(mainVC);
}
