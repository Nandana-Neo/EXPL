#!/usr/bin/bash

# FOLDER="ex2"
# Run ./script.sh <folder_name> <input_file> in shell
cd "$1"

yacc yacc.y -d
lex lex.l
gcc y.tab.c lex.yy.c -o a.out
./a.out < $2