%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "exptree.h"
    int yyerror();   
    int yylex();
%}
%token NUM;
%left '+' '-';
%left '*' '/';
%%
S   :   E '\n'      {   FILE * fp = fopen("exp.xsm","w");
                        fprintf(fp,"0\n2056\n0\n0\n0\n0\n0\n0\n");

                        codeGen($1, fp);

                        fprintf(fp,"MOV [4096], R0\n");
                        fprintf(fp,"MOV R1,\"Write\"\n");
                        fprintf(fp,"PUSH R1\n");
                        fprintf(fp,"MOV R1, -2\n");
                        fprintf(fp,"PUSH R1\n");
                        fprintf(fp,"PUSH R0\n");
                        fprintf(fp,"PUSH R0\n");
                        fprintf(fp,"PUSH R0\n");
                        fprintf(fp,"CALL 0\n");
                        fprintf(fp,"POP R0\nPOP R0\nPOP R0\nPOP R0\nPOP R0\n");
                        fprintf(fp,"MOV R1,\"Exit\"\n");
                        fprintf(fp,"PUSH R1\n");
                        fprintf(fp,"PUSH R1\n");
                        fprintf(fp,"PUSH R1\n");
                        fprintf(fp,"PUSH R1\n");
                        fprintf(fp,"PUSH R1\n");
                        fprintf(fp,"CALL 0\n");
                        
                        fprintf(stdout,"Done\n");
                        exit(0);
                    }
    ;
E   :   '+' ' ' E ' ' E     {
                        $$ = makeOperatorNode('+',$3,$5);
                    }
    |   '*' ' ' E ' ' E     {
                        $$ = makeOperatorNode('*',$3,$5);
                    }
    |   '/' ' ' E ' ' E     {
                        $$ = makeOperatorNode('/',$3,$5);
                    }
    |   '-' ' ' E ' ' E     {
                        $$ = makeOperatorNode('-',$3,$5);
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