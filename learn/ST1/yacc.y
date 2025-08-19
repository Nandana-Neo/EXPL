%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "exptree.h"
    int yyerror();   
    int yylex();
%}
%token NUM;
%left '+';
%%
S   :   E '\n'      {   printf("Prefix:");
                        prefix($1);
                        printf("\nPostfix:");
                        postfix($1);
                        exit(0);
                    }
    ;
E   :   E '+' E     {
                        $$ = makeOperatorNode('+',$1,$3);
                    }
    |   '(' E ')'   {
                        $$ = $2;
                    }
    |   NUM         {
                        $$ = $1;
                    } 
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