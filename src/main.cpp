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
#include <fstream>
#include <string>
#include <cstdio>

// Not sure if someone will ever use this, but the possible exit values are:
// -1: no parameters given. will display usage example.
//  0: not errors caught. yay!
//  1: something wrong while creating the output file
//  2: no input instance was given
//  3: input instance filename supplied, but couldn't open it
//  4: some invalid argument was given
//  5: invalid value for some valid option
//  6: something went wrong while reading instance
//  7: instance given is not solvable
int main(int argc, char *argv[]) {
  // arguments parsing
  float initialTemp, tempDecrease, tempStagesF, randomSeedF;
  int tempStages;
  unsigned int randomSeed;
  float *fptr;
  char *instanceFilename, *outputFilename;
  bool verbose = false;
  int firstOptionIndex;

  if  (argc <= 1) {
    printf("\nUsage example: \n");
    printf("1) sudoku <output file> <instance file> [options]\n");
    printf("2) sudoku <output file> [options]\n");
    printf("   => this second form will require the instance from stdin\n\n");
    printf("Options:\n");
    printf("  -it <N> \t: Initial temperature. Default: instance order * 100\n"); // NOLINT
    printf("  -a  <N> \t: Temperature decrease in each stage. Default: 0.9\n");
    printf("  -sl <N> \t: Temperature stages length. Default: (instance order)^2\n"); // NOLINT
    printf("  -rs <N> \t: Random seed. Default: [24]");
    printf("  -v      \t: Verbose switch. Will display input and output info to stdout");  // NOLINT
    printf("\n\n");
    return -1;
  }

  // check if is possible to create the output file
  std::ofstream ofile(argv[1]);
  if (!ofile) {
    printf("\nCouldn't create output file \"%s\"\n\n", argv[1]);
    return 1;
  }
  outputFilename = argv[1];

  if (ftell(stdin) >= 0) {  // if there is something at stdin, will read from it
    instanceFilename = '\0';
    firstOptionIndex = 2;
  } else {  // there is nothing at stdin, so we need some instance filename
    if ( argc == 2 ) {
      printf("\nMissing instance to solve.\n");
      return 2;
    } else {
      // check instance file
      std::ifstream ifile(argv[2]);
      if (!ifile) {
        printf("\nCouldn't open input file \"%s\"\n\n", argv[2]);
        return 3;
      }
      instanceFilename = argv[2];
      firstOptionIndex = 3;
    }
  }
  // read the rest
  // ps.: i know it's ugly, but i was in a kind of hurry to get this done
  initialTemp = tempDecrease = tempStagesF = randomSeedF = -1;
  std::string str;
  for (int i = firstOptionIndex; i < argc; ++i) {
    str = argv[i];
    if (str == "-v") {
      verbose = true;
      continue;
    } else if (str == "-it") {
      fptr = &initialTemp;
    } else if (str == "-alpha") {
      fptr = &tempDecrease;
    } else if (str == "-sl") {
      fptr = &tempStagesF;
    } else if (str == "-rs") {
      fptr = &randomSeedF;
    } else {
      printf("\nUnknown argument #%d \"%s\"\n", i, argv[i]);
      return 4;
    }
    if (i+1 < argc) {  // check if there is a i+1 arg
        // check if something went wrong reading it
        if (sscanf(argv[i+1], "%f", fptr) != 1) {  // NOLINT * avoids complaint about sscanf
          *fptr = -1;
          printf("Invalid value for \"%s\" argument (\"%s\")\n",
              argv[i], argv[i+1]);
          return 5;
        } else {
          ++i;  // next arg will be the value just assigned, so we can skip it
        }
    }
  }


  Sudoku s;
  switch (s.loadInstance(instanceFilename, true)) {
    case 0:
      printf("\nError processing instance.\n");
      return 6;
    case -1:
      printf("\nGiven instance is not possible to solve.\n");
      return 7;
  }

  printf("\nParameters:\n");

  printf("  instance file:\t");
  if (instanceFilename == '\0') {
    printf("stdin\n");
  } else {
    printf("%s\n", instanceFilename);
  }

  printf("  initial temp.:\t");
  if (initialTemp == -1) {
    printf("not set. Assuming default [instance order * 100].\n");
    initialTemp = s.order() * 100;
  } else {
    printf("%f\n", initialTemp);
  }

  printf("  alpha:\t\t");
  if (tempDecrease == -1) {
    printf("not set. Assuming default [0.9].\n");
    tempDecrease = 0.9;
  } else {
    printf("%f\n", tempDecrease);
  }

  printf("  stages length:\t");
  if (tempStagesF == -1) {
    printf("not set. Assuming default [(instance order)^2].\n");
    tempStages = s.order()*s.order();
  } else {
    tempStages = static_cast<int>(tempStagesF);
    printf("%d\n", tempStages);
  }

  printf("  random seed:\t\t");
  if (randomSeedF == -1) {
    printf("not set. Assuming default [24].\n");
    randomSeed = 24;
  } else {
    randomSeed = static_cast<unsigned int>(randomSeedF);
    printf("%d\n", randomSeed);
  }

  if (verbose) {
    printf("\n##############\n");
    printf("# Input info #\n");
    printf("##############\n");
    s.dump();
  }

  printf("\nSolving... ");
  int is, fs;
  double runtime;
  s.solve(initialTemp, tempDecrease, tempStages, randomSeed,
      &is, &fs, &runtime);
  if (fs == 0) {
    printf("OK!\n");
  } else {
    printf("Failed!\n");
  }

  printf("Execution info:\n");
  printf("  Initial Solution:\t%d\n", is);
  printf("  Final Solution:\t%d\n", fs);
  printf("  Runtime:\t\t~%fs\n\n", runtime);

  if (verbose) {
    printf("\n###############\n");
    printf("# Output info #\n");
    printf("###############\n");
    s.dump();
  }
  s.dumpBoardToFile(outputFilename);
  printf("Solution dumped to file \"%s\".", outputFilename);
  printf("\n");
  return 0;
}
