//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

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
    srand(app->currentMillis());
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

