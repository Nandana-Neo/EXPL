/*
    Infix to  prefix with strings
*/
%{
    #include <stdio.h>
    #include <string.h>
    #include "y.tab.h"
%}

%%
[a-zA-Z]+   {   
                yylval.c = strdup(yytext);
                return CH;
            }

[+*\n]      {   
                yylval.c = strdup(yytext); 
                return *yytext;
            }
%%

int yywrap(){
    return 1;
}