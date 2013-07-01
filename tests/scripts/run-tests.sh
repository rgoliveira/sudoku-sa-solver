#!/bin/bash

mkdir -p etc/out/
for i in {1..100}
do
    rs=$RANDOM
    for file in etc/*_*
    do
        ./bin/sudoku "etc/out/$(basename "$file").rs=$rs.out" -rs $rs < $file > "etc/out/$(basename "$file").rs=$rs.info"
    done
done

