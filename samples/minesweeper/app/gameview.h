//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#pragma once
#include "game.h"

class GameView : public View {
public:
    Game* _game;
    sp<Bitmap> _imgUnknown, _imgMine, _imgFlag, _imgBoom;
    sp<Bitmap> _imgNumbers[9];
    float _cellSize;
    sp<Timer> _timer;
    bool _wasLongPress;

    GameView();
    void setGame(Game* game);
    void updateCell(Cell& cell);

    // Overrides
    void updateIntrinsicSize(SIZE constrainingSize) override;
    bool handleInputEvent(INPUTEVENT* event) override;

protected:
    void processCellTouch(const POINT pt, bool longPress);
};
