/**************************DECLARATIONS*******************************/

%{
    #include <stdio.h>
    #include "y.tab.h"
%}
/**************************RULES****************************************/
%%
[a-zA-Z]    {   yylval.c = (*yytext);
                    return CH;
                }

[+*]        {    
                    return *yytext;
                }
\n          {   return *yytext;
            }
%%
/**************************AUXILIARY FNS********************************/
int yywrap(){
    return 1;
}