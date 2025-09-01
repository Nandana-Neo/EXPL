#!/usr/bin/bash

cd ..
yacc parser.y -d
lex lex.l
gcc -g y.tab.c lex.yy.c node/ast_node.c node/decl_node.c symbol_table/symbol_table.c code_gen/code_gen.c -o output

rm lex.yy.c
rm y.tab.c
rm y.tab.h

lex label_translation/label_transl.l
gcc -g lex.yy.c label_translation/label_transl.c -o label_translator

rm lex.yy.c