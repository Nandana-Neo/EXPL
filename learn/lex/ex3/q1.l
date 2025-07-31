%{
    #include <stdio.h>
    int lines = 0;
    int words = 0;
    int chars = 0;

%}

%%
\n  {lines++;words++;}
[ ] {words++;}
.   {chars++;}

%%
int yywrap(){
    lines++;
    return 1;
}
int main(){
    FILE * fp = fopen("input1.txt","r");
    if(fp)
        yyin = fp;
    yylex();
    printf("Lines:%d\nWords:%d\nChars:%d\n",lines,words,chars);
    return 0;
}