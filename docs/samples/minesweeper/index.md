---
layout: default
---
<iframe src="xx.html" style="margin-top:24px; margin-left:16px;" width="320" height="560" align="right">
</iframe>
# Minesweeper

A simple port of the classic game. Complete source code is reproduced below to
encourage casual browsing.

* [main.cpp](#main-cpp)
* [game.h](#game-h)
* [game.cpp](#game-cpp)
* [gameview.h](#gameview-h)
* [gameview.cpp](#gameview-cpp)


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


**game.h**
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


**game.cpp**
```cpp
#include "game.h"

Game::Game(Callback* callback, int rows, int cols) {
    _callback = callback;
    _rows = rows;
    _cols = cols;

    // Create an empty game board
    _cells = new Cell[rows * cols];
}

void Game::restart() {
    _state = InProgress;

    // Clear the board
    for (int r=0 ; r<_rows ; r++) {
        for (int c=0 ; c<_cols ; c++) {
            Cell& cell = cellAt(c,r);
            cell._minesAround = 0;
            cell._mine = false;
            cell._state = Unknown;
            _callback->onCellStateChanged(cell);
        }
    }

    // Fill board with mines at random locations
    srand(app.currentMillis());
    int mines = _rows * _cols / 7;
    _unknown = _rows * _cols - mines;
    for (int i = 0; i < mines; ++i) {
        int row = rand() % _rows;
        int col = rand() % _cols;
        Cell& cell = cellAt(col,row);
        if (cell._mine) {
            --i;
            continue;
        }
        cell._mine = true;
        for (int r=row-1 ; r<=row+1 ; r++) {
            for (int c=col-1 ; c<=col+1 ; c++) {
                if (r>=0 && r<_rows && c>=0 && c<_cols && !(r==row && c==col)) {
                    cellAt(c,r)._minesAround++;
                }
            }
        }
    }

    // Player starts with same number of flags as mines
    _flags = mines;

    // Fire all the callbacks cos everything changed
    _callback->onGameStateChanged();
    _callback->onGameNumFlagsChanged();
    _callback->onGameNumRemainingChanged();

}

Cell& Game::cellAt(int col, int row) {
    return _cells[row*_cols+col];
}

void Game::lose() {
    _state = Lost;
    _callback->onGameStateChanged();
}

void Game::winMaybe() {
    if (_unknown == 0 && _flags == 0) {
        _state = Won;
        _callback->onGameStateChanged();
    }
}

void Game::setCellState(Cell& cell, CellState newCellState) {
    if (cell._state != newCellState) {
        cell._state = newCellState;
        _callback->onCellStateChanged(cell);
    }
}

void Game::openCell(int col, int row) {
    Cell &cell = cellAt(col, row);
    if (cell._state == Unknown) {
        if (cell._mine) {
            setCellState(cell, Boom);
            lose();
        } else {
            revealCell(col, row);
        }
    }
}

void Game::revealCell(int col, int row) {
    Cell &cell = cellAt(col, row);
    if (cell._state == Unknown) {
        setCellState(cell, Open);
        _unknown--;
        _callback->onGameNumRemainingChanged();
        winMaybe();
        if (cell._minesAround == 0 && !cell._mine) {
            for (int r = row - 1; r <= row + 1; r++) {
                for (int c = col - 1; c <= col + 1; c++) {
                    if (r >= 0 && r < _rows && c >= 0 && c < _cols && !(col == c && row == r)) {
                        revealCell(c, r);
                    }
                }
            }
        }
    }
}

void Game::toggleFlag(int col ,int row) {
    Cell &cell = cellAt(col, row);
    if (cell._state == Flag) {
        setCellState(cell, Unknown);
        _flags++;
    } else if (cell._state == Unknown && _flags>0) {
        setCellState(cell, Flag);
        _flags--;
        winMaybe();
    }
    _callback->onGameNumFlagsChanged();
}
```


**gameview.h**
```cpp
#include "game.h"

class GameView : public View {
public:
    Game* _game;
    sp<BitmapProvider> _imgUnknown, _imgMine, _imgFlag, _imgBoom;
    sp<BitmapProvider> _imgNumbers[9];
    float _cellSize;
    sp<Timer> _timer;
    bool _wasLongPress;

    GameView();
    void setGame(Game* game);
    void updateCell(Cell& cell);

    // Overrides
    void updateContentSize(SIZE constrainingSize) override;
    bool handleInputEvent(INPUTEVENT* event) override;

protected:
    void processCellTouch(const POINT& pt, bool longPress);
};
```


**gameview.cpp**
```cpp
#include "gameview.h"

DECLARE_DYNCREATE(GameView);

GameView::GameView() {

    AtlasPage* atlas = new AtlasPage(512, 512, BITMAPFORMAT_RGBA32);
    _imgUnknown = atlas->importAsset("images/unknown.png");
    _imgMine = atlas->importAsset("images/mine.png");
    _imgBoom = atlas->importAsset("images/boom.png");
    _imgFlag = atlas->importAsset("images/flag.png");
    _imgNumbers[0] = atlas->importAsset("images/0.png");
    _imgNumbers[1] = atlas->importAsset("images/1.png");
    _imgNumbers[2] = atlas->importAsset("images/2.png");
    _imgNumbers[3] = atlas->importAsset("images/3.png");
    _imgNumbers[4] = atlas->importAsset("images/4.png");
    _imgNumbers[5] = atlas->importAsset("images/5.png");
    _imgNumbers[6] = atlas->importAsset("images/6.png");
    _imgNumbers[7] = atlas->importAsset("images/7.png");
    _imgNumbers[8] = atlas->importAsset("images/8.png");
    _cellSize = app.dp(25);
}

void GameView::setGame(Game* game) {
    _game = game;

    for (int y=0 ; y<_game->_rows ; y++) {
        for (int x=0 ; x<_game->_cols ; x++) {
            Cell &cell = _game->cellAt(x, y);
            cell._renderOp = new TextureRenderOp();
            cell._renderOp->setRect(RECT(x*_cellSize,y*_cellSize,_cellSize,_cellSize));
            updateCell(cell);
            addRenderOp(cell._renderOp);
        }
    }
}

void GameView::updateContentSize(SIZE constrainingSize) {
    _contentSize.width = _cellSize*_game->_cols;
    _contentSize.height = _cellSize*_game->_rows;
    _contentSizeValid = true;
}


void GameView::updateCell(Cell& cell) {
    if (cell._state == Unknown) {
        cell._renderOp->setBitmap(_imgUnknown);
    } else if (cell._state == Flag) {
        cell._renderOp->setBitmap(_imgFlag);
    } else if (cell._state == Mine) {
        cell._renderOp->setBitmap(_imgMine);
    } else if (cell._state == Boom) {
        cell._renderOp->setBitmap(_imgBoom);
    } else {
        cell._renderOp->setBitmap(_imgNumbers[cell._minesAround]);
    }
    cell._renderOp->rebatchIfNecessary();
    invalidateRect(cell._renderOp->_rect);
}


bool GameView::handleInputEvent(INPUTEVENT* event) {
    if (_game->_state != InProgress) {
        return false;
    }
    if (event->type == INPUT_EVENT_DOWN
        || event->type == INPUT_EVENT_DRAG
        || event->type == INPUT_EVENT_UP
        || event->type == INPUT_EVENT_CANCEL) {
        if (_timer) {
            _timer->stop();
            _timer = NULL;
        }
    }
    if (event->type == INPUT_EVENT_DOWN) {
        const POINT pt = event->ptLocal;
        _wasLongPress = false;
        _timer = Timer::start([=] () {
            _wasLongPress = true;
            processCellTouch(pt, true);
        }, 800, false);
    } else
    if (event->type == INPUT_EVENT_TAP) {
        if (!_wasLongPress) {
            processCellTouch(event->ptLocal, false);
        }
    }
    return true;
}


void GameView::processCellTouch(const POINT& pt, bool longPress) {
    int col = pt.x / _cellSize;
    int row = pt.y / _cellSize;
    if (col>=0 && row>=0 && col<_game->_cols && row<_game->_rows) {
        if (longPress) {
            _game->toggleFlag(col, row);
        } else {
            _game->openCell(col, row);
        }
    }
}
```
