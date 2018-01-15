//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "minesweepergame.h"

MinesweeperGame::MinesweeperGame(int rows, int cols) {
    _rows = rows;
    _cols = cols;

    // Create an empty game board
    _cells = new Cell[rows*cols];

    // Fill board with mines at random locations
    srand(oakCurrentMillis());
    _mines = rows * cols / 7;
    _emptyLeft = rows * cols - _mines;
    for (int i = 0; i < _mines; ++i) {
        int row = rand() % rows;
        int col = rand() % cols;
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
}

Cell& MinesweeperGame::cellAt(int col, int row) {
    return _cells[row*_cols+col];
}


