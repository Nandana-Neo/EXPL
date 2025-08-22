%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "exptree/exptree.h"
    #include "code_gen/code_gen.h"
    int yyerror();   
    int yylex();
    extern FILE* yyin;
%}
%token NUM ID P_BEGIN P_END READ WRITE;
%left '+' '-';
%left '*' '/';
%%
Program     : P_BEGIN Slist P_END   {
                                    FILE * fp = fopen("out.xsm","w");
                                    code_gen_start(fp);
                                    code_gen($2, fp);
                                    code_gen_final(fp);
                                    exit(0);
                                }
            | P_BEGIN P_END     {   
                                fprintf(stdout,"Emty program");
                                exit(0);
                            }
            ;

Slist       : Slist Stmt    {   $$ = make_operator_node('C',TYPE_NONE,$1,$2);   }
            | Stmt          {   $$ = $1;    }
            ;

Stmt        : InputStmt     {   $$ = $1;    }
            | OutputStmt    {   $$ = $1;    }
            | AsgStmt       {   $$ = $1;    }
            ;

InputStmt   : READ'('ID')'  {
                                $$ = make_operator_node(TYPE_NONE,'R',$3,NULL);
                            }
            ;

OutputStmt  : WRITE'('E')'  {  
                                $$ = make_operator_node(TYPE_NONE,'W',$3,NULL);
                            }
            ;

AsgStmt     : ID '=' E  {     
                            $$ = make_operator_node(TYPE_NONE,'=', $1, $3);
                        }
            ;
    
E   :   E '<' E 
    |   E '>' E 
    |   E '<''=' E 
    |   E '>''=' E 
    |   E != E | E == E;

    |   E '+' E     {
                        VarType type = TYPE_INT;
                        $$ = make_operator_node(type,'+',$1,$3);
                    }
    |   E '*' E     {
                        VarType type = TYPE_INT;
                        $$ = make_operator_node(type,'*',$1,$3);
                    }
    |   E '/' E     {
                        VarType type = TYPE_INT;
                        $$ = make_operator_node(type,'/',$1,$3);
                    }
    |   E '-' E     {
                        VarType type = TYPE_INT;
                        $$ = make_operator_node(type,'-',$1,$3);
                    }
    |   '(' E ')'   {
                        $$ = $2;
                    }
    |   NUM         {
                        $$ = $1;
                    } 
    |   ID          {
                        $$ = $1;
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