//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#pragma once
#include <oaknut.h>
#include "minesweepergame.h"

class MinesweeperView : public View {
public:
    MinesweeperGame* _game;
    ObjPtr<Bitmap> _imgUnopened, _imgMine, _imgFlag;
    ObjPtr<Bitmap> _imgNumbers[9];
    float _cellSize;
    Timer* _timer;
    bool _wasLongPress;

    MinesweeperView(MinesweeperGame* game);

    // Overrides
    virtual void updateContentSize(float parentWidth, float parentHeight);
    virtual bool onTouchEvent(int eventType, int finger, POINT pt);

protected:
    void loadImageAsset(ObjPtr<Bitmap>& rbitmap, const char* assetPath);
    void bindBitmaps();
    void updateCell(Cell& cell);
    void processCellTouch(const POINT& pt, bool longPress);
    void revealCell(int col, int row);
};
