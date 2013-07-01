# Compiler settings
CXX=g++
CXXFLAGS=-Wall -Iinclude

# Lint settings
LINT=python tools/cpplint.py
LINTFILTER=--filter=-readability/streams,-runtime/threadsafe_fn

# source files
SRC = src/main.cpp src/sudoku.cpp

MKDIR_P = @mkdir -p

OUT_DIR = bin

.PHONY: directories

# build
all: directories sudoku

directories: ${OUT_DIR}

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

sudoku:
	@echo .
	@echo Linting source files...
	$(LINT) $(LINTFILTER) $(SRC)
	@echo .
	@echo .
	@echo .
	@echo Compiling...
	$(CXX) -o bin/$@ $(SRC) $(CXXFLAGS)
	@echo .
