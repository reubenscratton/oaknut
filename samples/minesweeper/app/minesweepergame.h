//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#pragma once
#include <oaknut.h>

enum CellState {
    Unknown,
    Open,
    Flag,
    Mine
};

class Cell {
public:
    CellState _state;
    bool _mine;
    int _minesAround;
    ObjPtr<TextureRenderOp> _renderOp;

    Cell() : _state(Unknown), _mine(false), _minesAround(0) {
    }
};

class MinesweeperGame {
public:
    int _rows;
    int _cols;
    int _mines;
    int _emptyLeft;
    Cell* _cells;

    MinesweeperGame(int rows, int cols);
    Cell& cellAt(int col, int row);
};

