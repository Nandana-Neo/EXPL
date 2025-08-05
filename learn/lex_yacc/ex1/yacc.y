%{
    #include <stdio.h>
    #include <stdlib.h>
%}
%union {
    char c;
}
%token CH;
%left '+';
%left '*';
%%
start   :   expr '\n'       {printf("\nComplete\n");
                            exit(0);}
        ;
expr    :   expr '*' expr   {printf("* ");}
        |   expr '+' expr   {printf("+ ");}
        |   CH              {printf("%c ",$<c>1);}
        ;   

%%
int yyerror(){
    printf("Error\n");
    return 1;
}
int main(){
    yyparse();
    return 1;
}