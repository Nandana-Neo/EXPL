%option noyywrap
%{
    #include <stdio.h>
%}
%%
A[a-z|A-Z][a-z|A-Z] {printf("%s\n",yytext);}
(.|\n) {}
%%
int main(){
    FILE * fp = fopen("input3.txt","r");
    if(fp)
        yyin = fp;
    yylex();
    return 0;
}