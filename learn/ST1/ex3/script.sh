#!/usr/bin/bash

yacc yacc.y -d
lex lex.l
gcc y.tab.c lex.yy.c exptree.c -o a.out
./a.out

# Run `./xsm -l library.lib -e learn/ST1/ex3/exp.xsm` in console in parent folder xsm_expl