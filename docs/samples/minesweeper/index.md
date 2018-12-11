---
layout: default
---
<iframe src="xx.html" style="margin-top:24px; margin-left:16px;" width="320" height="560" align="right">
</iframe>
# Minesweeper

A simple port of the classic game.

**main.cpp**
```cpp
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
```


** main.h **
```cpp
#pragma once
#include <oaknut.h>

enum CellState {
    Unknown,
    Open,
    Flag,
    Mine,
    Boom
};

class Cell {
public:
    CellState _state;
    bool _mine;
    int _minesAround;
    sp<TextureRenderOp> _renderOp;

    Cell() : _state(Unknown), _mine(false), _minesAround(0) {
    }
};

enum GameState {
    InProgress,
    Won,
    Lost
};


class Game {
public:

    class Callback {
    public:
        virtual void onGameStateChanged()=0;
        virtual void onGameNumFlagsChanged()=0;
        virtual void onGameNumRemainingChanged()=0;
        virtual void onCellStateChanged(Cell& cell)=0;
    };

    GameState _state;
    int _rows;
    int _cols;
    int _flags;
    int _unknown;
    Cell* _cells;
    Callback* _callback;

    Game(Callback* _callback, int rows, int cols);
    Cell& cellAt(int col, int row);
    void restart();
    void lose();
    void winMaybe();
    void openCell(int col, int row);
    void revealCell(int col, int row);
    void toggleFlag(int col ,int row);
    void setCellState(Cell& cell, CellState newCellState);
};
```
