%{
    #include <stdio.h>
    int yyerror();
    #include <stdlib.h>
%}

%token NUM;
%token ID;
%left '+';
%left '=';
%%
START : S '\n'      {printf("\nAccept\n"); exit(0);}
S   :   ID '='  E   {}
    |   ID '+''=' E {}
    ;
E   :   E '+' E     {}
    |   ID          {}
    |   NUM         {}
    ;
%%
int yyerror(){
    printf("\nRejected\n");
    return 1;
}
int main(){
    yyparse();
    return 1;
}