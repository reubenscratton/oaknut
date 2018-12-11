//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

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
