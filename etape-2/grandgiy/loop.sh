#!/bin/bash
for dir in ../tests/avances/*; do for i in {0..199}; do echo "Parsing de $dir/$i.txt :"; ./test $dir/$i.txt; echo "------------------------------------"; done; done