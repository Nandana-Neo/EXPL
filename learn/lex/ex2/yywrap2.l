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
        yyin = stdin;
        return 0;
    }
    else
        return 1;
}

int main(){
    FILE *newfile_pointer = fopen("input_file.l","r");
    if(newfile_pointer)
        yyin = newfile_pointer;
    yylex();
    return 1;
}