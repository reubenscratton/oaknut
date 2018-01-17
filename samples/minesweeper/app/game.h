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
    Mine,
    Boom
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

