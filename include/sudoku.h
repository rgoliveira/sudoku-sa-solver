/* Copyright 2013 Ricardo Godoy de Oliveira
 *
 * ----------------------------------------------------------------------------
 * This file is part of Sudoku SA Solver.
 *
 * Sudoku SA Solver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sudoku SA Solver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * <http://www.gnu.org/licenses/>
 * ----------------------------------------------------------------------------
 * This project is part of an assignment for the course of INF05010 (Combina-
 * torial Optimization) at the Informatics Institute of the Federal University
 * of Rio Grando do Sul (UFRGS). It's aim is to solve instances of Sudoku using
 * the meta-heuristic known as Simulated Annealing.
 * Along with this program, there should be a technical report containing the
 * results of the experimentation against some instances of the game, as well
 * as information about the meta-heuristic and algorithms used.
 * ----------------------------------------------------------------------------
 */

#include <vector>
#include <cstdio>

typedef std::vector<std::vector<int> > TBoard;
typedef std::vector<std::vector<bool> > TBoardBool;

// Representes a Sudoku game.
// Loads instances from files and solve them using Simulated Annealing.
class Sudoku {
  public:
    // accessers
    TBoard board()  { return _board; }
    int order()     { return _order; }

    // constructor & destructor
    Sudoku ();
    ~Sudoku ();

    // ***********
    // * methods *
    // ***********

    // Loads instance from file, setting _board and _fixeds.
    int loadInstance(const char* filename, bool checkValidity = true);

    bool isCellFixed(int i, int j);

    // Generate initial solution
    // (see function definition for details on how it it's done)
    void genInitSolution();

    void swapCells(int i1, int j1, int i2, int j2);

    bool solve(float initialTemp, float alpha, int stagesLength,
        unsigned int randomSeed, int* initialSolution, int* finalSolution,
        double* seconds);

    int evaluateCurrentSolution();

    // Dump some current data.
    void dump();
    void dumpBoardToFile(const char* filename);
  private:
    TBoard      _board;     // represents the board itself
    TBoardBool  _fixeds;    // flags fixed cells
    int         _order;     // boards order = board will have a size of (order^2 x order^2)
    int         _n;         // holds order*order to avoid squaring _order everytime
};
