%{
    #include<stdio.h>
    int cnt = 0;
%}

%%

[0-9]+  printf("number");

%%

int yywrap(){
    if(!cnt){
        cnt=1;
        return 0;
    }
    else
        return 1;
}

int main(){
    yylex();
    return 1;
}