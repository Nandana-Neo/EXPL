%{
    #include <stdio.h>
    #include "y.tab.h"
%}
num [0-9]
letter  [a-zA-Z]
%%
{num}+                      {   return NUM; }
{letter}[{num}|{letter}]*   {   return ID;  }
[+=\n]                      {   return *yytext; }
(.)                         {   printf("Error\n"); exit(1);}
%%
int yywrap(){
    return 1;
}