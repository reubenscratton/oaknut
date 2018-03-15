//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "gameview.h"

DECLARE_DYNCREATE(GameView);

GameView::GameView() {
    _imgUnknown = new AsyncBitmapProvider("images/unknown.png");
    _imgMine = new AsyncBitmapProvider("images/mine.png");
    _imgBoom = new AsyncBitmapProvider("images/boom.png");
    _imgFlag = new AsyncBitmapProvider("images/flag.png");
    _imgNumbers[0] = new AsyncBitmapProvider("images/0.png");
    _imgNumbers[1] = new AsyncBitmapProvider("images/1.png");
    _imgNumbers[2] = new AsyncBitmapProvider("images/2.png");
    _imgNumbers[3] = new AsyncBitmapProvider("images/3.png");
    _imgNumbers[4] = new AsyncBitmapProvider("images/4.png");
    _imgNumbers[5] = new AsyncBitmapProvider("images/5.png");
    _imgNumbers[6] = new AsyncBitmapProvider("images/6.png");
    _imgNumbers[7] = new AsyncBitmapProvider("images/7.png");
    _imgNumbers[8] = new AsyncBitmapProvider("images/8.png");

    _cellSize = app.dp(25);
}

void GameView::setGame(Game* game) {
    _game = game;

    for (int y=0 ; y<game->_rows ; y++) {
        for (int x=0 ; x<game->_cols ; x++) {
            Cell &cell = game->cellAt(x, y);
            cell._renderOp = new TextureRenderOp(this);
            cell._renderOp->setRect(RECT_Make(x*_cellSize,y*_cellSize,_cellSize,_cellSize));
            addRenderOp(cell._renderOp);
        }
    }
}

void GameView::updateContentSize(float parentWidth, float parentHeight) {
    _contentSize.width = _cellSize*_game->_cols;
    _contentSize.height = _cellSize*_game->_rows;
    _contentSizeValid = true;
}


void GameView::updateCell(Cell& cell) {
    if (cell._state == Unknown) {
        cell._renderOp->setBitmapProvider(_imgUnknown);
    } else if (cell._state == Flag) {
        cell._renderOp->setBitmapProvider(_imgFlag);
    } else if (cell._state == Mine) {
        cell._renderOp->setBitmapProvider(_imgMine);
    } else if (cell._state == Boom) {
        cell._renderOp->setBitmapProvider(_imgBoom);
    } else {
        cell._renderOp->setBitmapProvider(_imgNumbers[cell._minesAround]);
    }
    cell._renderOp->rebatchIfNecessary();
    invalidateRect(cell._renderOp->_rect);
}


bool GameView::onTouchEvent(int eventType, int finger, POINT pt) {
    if (_game->_state != InProgress) {
        return false;
    }
    if (eventType == INPUT_EVENT_DOWN
        || eventType == INPUT_EVENT_DRAG
        || eventType == INPUT_EVENT_UP
        || eventType == INPUT_EVENT_CANCEL) {
        if (_timer) {
            _timer->stop();
            _timer = NULL;
        }
    }
    if (eventType == INPUT_EVENT_DOWN) {
        _wasLongPress = false;
        _timer = Timer::start([=] () {
            _wasLongPress = true;
            processCellTouch(pt, true);
        }, 800, false);
    } else
    if (eventType == INPUT_EVENT_TAP) {
        if (!_wasLongPress) {
            processCellTouch(pt, false);
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

