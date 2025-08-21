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
    yylval = createTree(val, TYPE_INT, NULL, NULL, NULL);
    return NUM;
}

[a-z]   {
    char* variable = strdup(yytext);    // ** Value is assumed to be int
    yylval = createTree(0, TYPE_INT, variable, NULL, NULL);
    return ID;
}

[()+*/=-] {
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