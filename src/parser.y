%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "exptree.h"
    int yyerror();   
    int yylex();
    extern FILE* yyin;
%}
%token NUM ID P_BEGIN P_END READ WRITE;
%left '+' '-';
%left '*' '/';
%%
Program     : P_BEGIN Slist P_END   {
                                    fprintf(stdout,"Done\n");
                                    prefix($2);
                                    exit(0);
                                }
            | P_BEGIN P_END     {   fprintf(stdout,"Emty program");
                                exit(0);
                            }
            ;

Slist       : Slist Stmt    {   $$ = makeOperatorNode('C',$1,$2);   }
            | Stmt          {   $$ = $1;    }
            ;

Stmt        : InputStmt     {   $$ = $1;    }
            | OutputStmt    {   $$ = $1;    }
            | AsgStmt       {   $$ = $1;    }
            ;

InputStmt   : READ'('ID')'  {
                                $$ = makeOperatorNode('R',$3,NULL);
                            }
            ;

OutputStmt  : WRITE'('E')'  {  
                                $$ = makeOperatorNode('W',$3,NULL);
                            }
            ;

AsgStmt     : ID '=' E  {     
                            $$ = makeOperatorNode('=', $1, $3);
                        }
            ;
    
E   :   E '+' E     {
                        $$ = makeOperatorNode('+',$1,$3);
                    }
    |   E '*' E     {
                        $$ = makeOperatorNode('*',$1,$3);
                    }
    |   E '/' E     {
                        $$ = makeOperatorNode('/',$1,$3);
                    }
    |   E '-' E     {
                        $$ = makeOperatorNode('-',$1,$3);
                    }
    |   '(' E ')'   {
                        $$ = $2;
                    }
    |   NUM         {
                        $$ = $1;
                    } 
    |   ID          {   $$ = $1;
                    }
    ;
%%
int yyerror(){
    printf("Error\n");
    return 1;
}
int main(int argc, char* argv[]){
    if(argc > 1){
        yyin = fopen(argv[1],"r");
        if(!yyin){
            perror("fopen");
            return 1;
        }
    }
    yyparse();
    return 1;
}