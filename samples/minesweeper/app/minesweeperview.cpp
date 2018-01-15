//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "minesweeperview.h"

MinesweeperView::MinesweeperView(MinesweeperGame* game) {
    _game = game;
    loadImageAsset(_imgUnopened, "images/unopened.png");
    loadImageAsset(_imgMine, "images/mine.png");
    loadImageAsset(_imgFlag, "images/flag.png");
    loadImageAsset(_imgNumbers[0], "images/0.png");
    loadImageAsset(_imgNumbers[1], "images/1.png");
    loadImageAsset(_imgNumbers[2], "images/2.png");
    loadImageAsset(_imgNumbers[3], "images/3.png");
    loadImageAsset(_imgNumbers[4], "images/4.png");
    loadImageAsset(_imgNumbers[5], "images/5.png");
    loadImageAsset(_imgNumbers[6], "images/6.png");
    loadImageAsset(_imgNumbers[7], "images/7.png");
    loadImageAsset(_imgNumbers[8], "images/8.png");

    _cellSize = dp(25);

    for (int y=0 ; y<game->_rows ; y++) {
        for (int x=0 ; x<game->_cols ; x++) {
            Cell &cell = game->cellAt(x, y);
            cell._renderOp = new TextureRenderOp(this);
            cell._renderOp->setRect(RECT_Make(x*_cellSize,y*_cellSize,_cellSize,_cellSize));
            addRenderOp(cell._renderOp);
        }
    }

}

void MinesweeperView::updateContentSize(float parentWidth, float parentHeight) {
    _contentSize.width = _cellSize*_game->_cols;
    _contentSize.height = _cellSize*_game->_rows;
    _contentSizeValid = true;
}


void MinesweeperView::loadImageAsset(ObjPtr<Bitmap>& rbitmap, const char* assetPath) {
    Data* data = oakLoadAsset(assetPath);
    oakBitmapCreateFromData(data->data, (int)data->cb, [&](Bitmap* bitmap) {
        rbitmap = bitmap;
        bindBitmaps();
    });
}

void MinesweeperView::bindBitmaps() {
    for (int y=0 ; y<_game->_rows ; y++) {
        for (int x = 0; x < _game->_cols; x++) {
            Cell &cell = _game->cellAt(x, y);
            updateCell(cell);
        }
    }
    setNeedsFullRedraw();
}

void MinesweeperView::updateCell(Cell& cell) {
    if (cell._state == Unknown) {
        cell._renderOp->_bitmap = _imgUnopened;
    } else if (cell._state == Flag) {
        cell._renderOp->_bitmap = _imgFlag;
    } else if (cell._state == Mine) {
        cell._renderOp->_bitmap = _imgMine;
    } else {
        cell._renderOp->_bitmap = _imgNumbers[cell._minesAround];
    }
    cell._renderOp->rebatchIfNecessary();
    invalidateRect(cell._renderOp->_rect);
}


bool MinesweeperView::onTouchEvent(int eventType, int finger, POINT pt) {
    if (eventType == TOUCH_EVENT_DOWN
        || eventType == TOUCH_EVENT_DRAG
        || eventType == TOUCH_EVENT_UP
        || eventType == TOUCH_EVENT_CANCEL) {
        if (_timer) {
            _timer->stop();
            _timer = NULL;
        }
    }
    if (eventType == TOUCH_EVENT_DOWN) {
        _wasLongPress = false;
        _timer = Timer::start([=] () {
            _wasLongPress = true;
            processCellTouch(pt, true);
        }, 800, false);
    } else
    if (eventType == TOUCH_EVENT_TAP) {
        if (!_wasLongPress) {
            processCellTouch(pt, false);
        }
    }
    return true;
}


void MinesweeperView::processCellTouch(const POINT& pt, bool longPress) {
    int col = pt.x / _cellSize;
    int row = pt.y / _cellSize;
    if (col>=0 && row>=0 && col<_game->_cols && row<_game->_rows) {
        Cell &cell = _game->cellAt(col, row);
        if (longPress) {
            if (cell._state == Flag) {
                cell._state = Unknown;
            } else if (cell._state == Unknown) {
                cell._state = Flag;
            }
        } else {
            if (cell._state == Unknown) {
                if (cell._mine) {
                    cell._state = Mine;
                    // todo: boom!
                } else {
                    revealCell(col, row);
                }
            }
        }
        updateCell(cell);
    }
}

void MinesweeperView::revealCell(int col, int row) {
    Cell &cell = _game->cellAt(col, row);
    if (cell._state == Unknown && !cell._mine) {
        cell._state = Open;
        updateCell(cell);
        if (cell._minesAround == 0) {
            for (int r = row - 1; r <= row + 1; r++) {
                for (int c = col - 1; c <= col + 1; c++) {
                    if (r >= 0 && r < _game->_rows && c >= 0 && c < _game->_cols && !(col == c && row == r)) {
                        //Cell &edgeCell = _game->cellAt(c, r);
                        revealCell(c, r);
                    }
                }
            }
        }
    }

}