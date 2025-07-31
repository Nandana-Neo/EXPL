%option noyywrap
%{
    #include <stdio.h>
    int integers = 0;
    int floats = 0;

%}

%%
[0-9]+ {integers++;}
[0-9]+"."[0-9]+ {floats++;}
(.) {}
%%
int main(){
    FILE * fp = fopen("input2.txt","r");
    if(fp)
        yyin = fp;
    yylex();
    printf("Integers:%d\nFloats:%d\n",integers,floats);
    return 0;
}