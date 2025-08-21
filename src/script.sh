#!/usr/bin/bash

# $1 = input file name 

yacc parser.y -d
lex lex.l
gcc y.tab.c lex.yy.c exptree/exptree.c code_gen/code_gen.c -o a.out
./a.out $1

# Run `./xsm -l library.lib -e learn/ST1/exp.xsm` in console in parent folder xsm_expl