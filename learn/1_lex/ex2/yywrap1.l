%{
    #include<stdio.h>
    int cnt = 0;
%}

%%

[0-9]+  printf("number");

%%

int yywrap(){
    FILE *newfile_pointer = fopen("input_file.l","r");
    if(!cnt){
        cnt=1;
        yyin = newfile_pointer;
        return 0;
    }
    else
        return 1;
}

int main(){
    yylex();
    return 1;
}