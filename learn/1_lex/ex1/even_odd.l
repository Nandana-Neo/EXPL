%option noyywrap
%{
    #include <stdio.h>
%}

odd ([0-9]*)[1|3|5|7|9]
even ([0-9]*)[0|2|4|6|8]

%%

{odd}   {printf("odd\n");}
{even}  {printf("even\n"); return 1;}

%%
int main(){
    yylex();
    return 0;
}