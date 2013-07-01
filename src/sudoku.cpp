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

#include <sudoku.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

Sudoku::Sudoku() {
  _order = -1;
  _n = -1;
  _board.clear();
  _fixeds.clear();
}

Sudoku::~Sudoku() {}

// If checkValidity is true, will check whether the instance is solvable or not.
// Returns:
//  1: everything went good;
//  0: something went wrong while reading the file;
// -1: only whit checkValidity set. loaded the instance, but it is not solvable.
int Sudoku::loadInstance(const char* filename, bool checkValidity) {
  int buffer;
  FILE* fin;
  if (filename == '\0') {  // if no filename is given, will try to read stdin
    fin = stdin;
  } else {
    fin = fopen(filename, "r");
    if (fin == NULL) {
      printf("Failed to read file \"%s\"\n", filename);
      return 0;
    }
  }
  if (fscanf(fin, "%d", &_order) != 1) {
    printf("Invalid board size.\n");
    return 0;
  }
  assert(_order > 0 && _order < 50);
  _n = _order * _order;
  for (int i = 0; i < _n; ++i) {
    _board.push_back(std::vector<int>());
    _fixeds.push_back(std::vector<bool>());
    for (int j = 0; j < _n; ++j) {
      fscanf(fin, "%d", &buffer);
      _board[i].push_back(buffer);
      _fixeds[i].push_back(buffer > -1);
    }
  }
  if (!checkValidity) {
    return 1;
  } else {
    //                / true, if 'w' was already used in the current square
    // usedNumbers[w]|
    //                \ false otherwise.
    std::vector<bool> usedNumbers;

    // check rows
    for (int i = 0; i < _n; ++i) {
      usedNumbers = std::vector<bool>(_n+1, false);
      for (int j = 0; j < _n; ++j) {
        if (isCellFixed(i, j)) {
          // already used this number in this row?
          if (usedNumbers[_board[i][j]]) {
            printf("\nr(%d,%d)\n", i, j);
            return -1;
          } else {
            usedNumbers[_board[i][j]] = true;
          }
        }
      }
    }
    // check columns
    for (int j = 0; j < _n; ++j) {
      usedNumbers = std::vector<bool>(_n+1, false);
      for (int i = 0; i < _n; ++i) {
        if (isCellFixed(i, j)) {
          // already used this number in this column?
          if (usedNumbers[_board[i][j]]) {
            printf("\nc(%d,%d)\n", i, j);
            return -1;
          } else {
            usedNumbers[_board[i][j]] = true;
          }
        }
      }
    }
    return 1;
  }
}

bool Sudoku::isCellFixed(int i, int j) {
  assert(_order > 0);
  assert(i >= 0 && i < _n);
  assert(j >= 0 && j < _n);
  return _fixeds[i][j];
}

// Generate an initial solution, respecting:
//  1.  The fixed cells and
//  2.  The third restriction of the game, which constraints each order*order
//      square to have each integer in [1, n^2] exactly once.
void Sudoku::genInitSolution() {
  //                / true, if 'w' was already used in the current square
  // usedNumbers[w]|
  //                \ false otherwise.
  std::vector<bool> usedNumbers;
  int x;
  int aux = 0;
  for (int i = 0; i < _order; ++i) {        // vertical position of the square
    for (int j = 0; j < _order; ++j) {      // horizontal position of the square
      usedNumbers = std::vector<bool>(_n+1, false);

      // loop to find fixed values inside this square (i,j)
      for (int p = 0; p < _order; ++p) {    // line inside the square
        for (int q = 0; q < _order; ++q) {  // column inside the square
          if (isCellFixed(_order*i + p, _order*j + q)) {
            aux = _board[_order*i + p][_order*j + q];
            usedNumbers[aux] = true;
            // printf("pos(%d,%d) = %d (fixed)\n",
            //   _order*i + p, _order*j + q, aux);
          }
        }
      }

      // loop to fill the nonfixed cells,
      // subject to the third constraint of the game.
      x = 0;
      for (int p = 0; p < _order; ++p) {    // line inside the square
        for (int q = 0; q < _order; ++q) {  // column inside the square
          if (!isCellFixed(_order*i + p, _order*j + q)) {
            aux = (x % _n) + 1;  // candidate value to fill the cell
            // printf("pos(%d,%d) = %d\n", _order*i + p, _order*j + q, aux);
            while (usedNumbers[aux]) {  // find the first available value.
              aux = (++x % _n) + 1;     // note: it may not entry here if
                                        // the candidate was ok.
              // printf("x=%d;", x);
            }
            _board[_order*i + p][_order*j + q] = aux;
            usedNumbers[aux] = true;
            ++x;
          }
        }
      }
    }
  }
}

void Sudoku::swapCells(int i1, int j1, int i2, int j2) {
  // swap only if they're not fixed
  if (!isCellFixed(i1, j1) && !isCellFixed(i2, j2)) {
    std::swap(_board[i1][j1], _board[i2][j2]);
  }
}

// Solve current board using Simulated Annealing.
// Outputs are:
//  initialSolution : value of the first generated solution;
//  finalSolution   : value of the final solution;
//  seconds         : seconds of runtime to reach the stop criteria.
bool Sudoku::solve(float initialTemp, float alpha, int stagesLength,
    unsigned int randomSeed, int* initialSolution, int* finalSolution,
    double* seconds) {
  // cell selection variables
  int i, j;             // selected square
  int p1, q1, p2, q2;   // selected cells (p1,q1) and (p2,q2)
  // simulated annealing variables
  int curSolution, candidateSolution, deltaS;
  float temperature;
  double prob, r;
  // runtime measurement variables
  clock_t begin, end;

  begin = clock();

  genInitSolution();
  temperature = initialTemp;
  curSolution = evaluateCurrentSolution();
  *initialSolution = curSolution;
  srand(randomSeed);
  while (curSolution > 0) {
    // loop controling temperature stages length
    for (int stageI = 0; stageI < stagesLength && curSolution > 0; ++stageI) {

      // selection of the two non-fixed cells (in the same square)
      // note: the way it ignores the selection may cause it to rarely deal with
      //       squares containing very few non-fixeds.
      do {
        // select a square
        i = rand() % _order;
        j = rand() % _order;

        // select two cells in the selected square
        p1 = p2 = i*_order;
        q1 = q2 = j*_order;
        p1 += rand() % _order;
        q1 += rand() % _order;
        p2 += rand() % _order;
        q2 += rand() % _order;
      } while (isCellFixed(p1, q1) || isCellFixed(p2, q2));

      // swap them
      // printf("swapping (%d,%d) <-> (%d,%d)\n", p1, q1, p2, q2);
      swapCells(p1, q1, p2, q2);
      candidateSolution = evaluateCurrentSolution(); // todo: delta evaluation
      // printf("from % 3d to % 3d\n", curSolution, candidateSolution);

      if (candidateSolution <= curSolution) {
        curSolution = candidateSolution;
        // printf("solution updated to %d (BETTER)\n", curSolution);
      } else {
        deltaS = candidateSolution - curSolution;
        prob = exp(-1*(deltaS/temperature));
        r = static_cast<double>(rand()) / (RAND_MAX);
        // printf("prob => e^-(%d/%f) = %f  |  r = %f\n",
        //     deltaS, temperature, prob, r);
        if ( r <= prob ) {
          curSolution = candidateSolution;
          // printf("solution updated to %d (PROB)\n", curSolution);
        } else {
          // swap back to place
          swapCells(p1, q1, p2, q2);
        }
      }
    }
    temperature *= alpha;
  }

  end = clock();
  *seconds = static_cast<double>(end-begin)/CLOCKS_PER_SEC;
  *finalSolution = curSolution;

  return (curSolution == 0);
}

// print some information to stdout
void Sudoku::dump() {
  printf(" => Order: %d\n", _order);
  printf(" => Current Cost: ");
  int cost = evaluateCurrentSolution();
  if (cost >= 0) {
    printf("%d\n", cost);
  } else {
    printf("board not properly filled yet.\n");
  }

  // draw board
  printf(" => Board:\n");
  printf("----");
  for (int j = 0; j < _n; ++j)
    printf("----");
  printf("\n");
  for (int i = 0; i < _n; ++i) {
    for (int j = 0; j < _n; ++j) {
      if (j % _order == 0)
        printf("|");
      if (_board[i][j] > 0)
        printf("% 3d ", _board[i][j]);
      else
        printf(" __ ");
    }
    printf("|\n");
    if ((i+1) % _order == 0) {
      printf("----");
      for (int j = 0; j < _n; ++j)
        printf("----");
    }
    printf("\n");
  }
}

// Dumps board to file using the same format as the input.
void Sudoku::dumpBoardToFile(const char* filename) {
  std::ofstream output(filename);
  if (output) {
    output << _order << std::endl;
    for (int i = 0; i < _n; ++i) {
      for (int j = 0; j < _n; ++j) {
        output << _board[i][j] << " ";
      }
      output << std::endl;
    }
  }
}

// Evaluates the cost of the current configuration.
// How? Returns the SUM of the REPEATED values in each row AND each column.
int Sudoku::evaluateCurrentSolution() {
  //                / true, if 'w' was already used in the current analysis
  // usedNumbers[w]|
  //                \ false otherwise.
  std::vector<bool> usedNumbers;
  int cost = 0;

  // analyzes rows
  for (int i = 0; i < _n; ++i) {
    usedNumbers = std::vector<bool>(_n+1, false);
    for (int j = 0; j < _n; ++j) {
      // if board is not properly filled, exit
      if (_board[i][j] < 1 || _board[i][j] > _n) return -1;
      if (usedNumbers[_board[i][j]]) {
        ++cost;
      } else {
        usedNumbers[_board[i][j]] = true;
      }
    }
  }

  // analyzes columns
  for (int j = 0; j < _n; ++j) {
    usedNumbers = std::vector<bool>(_n+1, false);
    for (int i = 0; i < _n; ++i) {
      if (usedNumbers[_board[i][j]]) {
        ++cost;
      } else {
        usedNumbers[_board[i][j]] = true;
      }
    }
  }
  return cost;
}
