%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "exptree/exptree.h"
    #include "y.tab.h"
    int yywrap();
%}

%%
[0-9]+  {
    int val = atoi(yytext);
    yylval = make_leaf_node(val, TYPE_INT, NULL);
    return NUM;
}

[a-z]   {
    char* variable = strdup(yytext);    // ** Value is assumed to be int
    yylval = make_leaf_node(0, TYPE_INT, variable);
    return ID;
}

[()+*/=-<>!] {
    return *yytext;
}

"begin"     {   return P_BEGIN;   }
"end"       {   return P_END;     }
"read"      {   return READ;    }
"write"     {   return WRITE;   }

%%
int yywrap(){
    return 1;
}