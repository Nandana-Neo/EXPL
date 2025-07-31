%option noyywrap
%{
    #include <stdio.h>
%}
%%
"//"(.)* {printf("Comment found:%s\n",yytext);}
"/*"(.|\n)*"*/"  {printf("Comment found:%s\n",yytext);}
(.|\n)  {}
%%
int main(){
    FILE* fp = fopen("input4.txt","r");
    if(fp){
        yyin = fp;
    }
    yylex();
    return 0;
}