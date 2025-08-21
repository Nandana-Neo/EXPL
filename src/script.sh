#!/usr/bin/bash

# $1 = input file name 

yacc yacc.y -d
lex lex.l
gcc y.tab.c lex.yy.c exptree.c -o a.out
./a.out $1

# Run `./xsm -l library.lib -e learn/ST1/exp.xsm` in console in parent folder xsm_expl