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

    ObjPtr<BitmapProvider> _bmpFaceNormal;
    ObjPtr<BitmapProvider> _bmpFaceWon;
    ObjPtr<BitmapProvider> _bmpFaceLost;

    MainViewController() {

        _bmpFaceNormal = new AsyncBitmapProvider("images/face_normal.png");
        _bmpFaceWon = new AsyncBitmapProvider("images/face_won.png");
        _bmpFaceLost = new AsyncBitmapProvider("images/face_lost.png");

        View* view = Styles::layoutInflate("layout/main.res");
        _gameView = (GameView*)view->findViewById("game");
        _labelNumFlags = (Label*)view->findViewById("flags");
        _imageViewFace = (ImageView*)view->findViewById("face");
        _imageViewFace->onTouchEventDelegate = [=](View* view, int eventType, int eventSource, POINT pt) -> bool {
            if (eventType == INPUT_EVENT_TAP) {
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
                _imageViewFace->setBitmapProvider(_bmpFaceNormal);
                break;
            case Won:
                _imageViewFace->setBitmapProvider(_bmpFaceWon);
                break;
            case Lost:
                _imageViewFace->setBitmapProvider(_bmpFaceLost);
                break;
        }
    }
    virtual void onGameNumFlagsChanged() {
        _labelNumFlags->setText("Mines: %d", _game->_flags);
    }
    virtual void onGameNumRemainingChanged() {
        _labelNumRemaining->setText("Closed: %d", _game->_unknown);
    }
    virtual void onCellStateChanged(Cell& cell) {
        _gameView->updateCell(cell);
    }

};

void App::main() {
    Styles::loadAsset("styles.res");

    MainViewController* mainVC = new MainViewController();
    _window->setRootViewController(mainVC);
}


