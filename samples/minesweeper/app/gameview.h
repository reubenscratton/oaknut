//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#pragma once
#include <oaknut.h>
#include "game.h"

class GameView : public View {
public:
    Game* _game;
    ObjPtr<BitmapProvider> _imgUnknown, _imgMine, _imgFlag, _imgBoom;
    ObjPtr<BitmapProvider> _imgNumbers[9];
    float _cellSize;
    ObjPtr<Timer> _timer;
    bool _wasLongPress;

    GameView();
    void setGame(Game* game);
    void updateCell(Cell& cell);

    // Overrides
    virtual void updateContentSize(float parentWidth, float parentHeight);
    virtual bool onTouchEvent(int eventType, int finger, POINT pt);

protected:
    void processCellTouch(const POINT& pt, bool longPress);
};
