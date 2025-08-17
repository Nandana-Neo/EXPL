%{
    #include <stdio.h>
    #include <stdlib.h>
%}
%union {
    char * c;
}
%token CH;
%left '+';
%left '*';
%%
start   :   expr '\n'       {
                                printf("\nExpr:%s\nComplete\n",$<c>1);
                                exit(0);
                            }
        ;
expr    :   expr '*' expr   {
                                strcat($<c>2," ");
                                strcat($<c>2,$<c>1);
                                strcat($<c>2," ");
                                strcat($<c>2,$<c>3);
                                free($<c>1);
                                free($<c>3);
                                $<c>$ = $<c>2;
                            }

        |   expr '+' expr   {
                                strcat($<c>2," ");
                                strcat($<c>2,$<c>1);
                                strcat($<c>2," ");
                                strcat($<c>2,$<c>3);
                                free($<c>1);
                                free($<c>3);
                                $<c>$ = $<c>2;
                            }

        |   CH              {
                                $<c>$ = $<c>1;
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