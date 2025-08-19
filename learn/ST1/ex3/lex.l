%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "exptree.h"
    #include "y.tab.h"
    int yywrap();
%}

%%
[0-9]+  {
    int val = atoi(yytext);
    yylval = makeLeafNode(val);
    return NUM;
}

[()+*/\n- ] {
    return *yytext;
}

%%
int yywrap(){
    return 1;
}