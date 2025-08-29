%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "exptree/exptree.h"
    #include "code_gen/code_gen.h"
    int yyerror();   
    int yylex();
    extern FILE* yyin;
    FILE * output_file;
%}
%token NUM ID P_BEGIN P_END READ WRITE IF THEN ELSE ENDIF WHILE DO ENDWHILE BREAK CONTINUE REPEAT UNTIL;
%left '+' '-';
%left '*' '/';
%nonassoc '<' '>' '=' ';';
%%
Program     : P_BEGIN Slist P_END   {
                                    FILE * fp = output_file;
                                    code_gen_start(fp);
                                    code_gen($2, fp, -1, -1);
                                    code_gen_final(fp);
                                    // evaluate($2);
                                    exit(0);
                                }
            | P_BEGIN P_END     {   
                                fprintf(stdout,"Emty program");
                                exit(0);
                            }
            ;

Slist       : Slist Stmt    {   $$ = make_operator_node(TYPE_NONE,NODE_CONN,$1,$2);   }
            | Stmt          {   $$ = $1;    }
            ;

Stmt        : InputStmt ';'     {   $$ = $1;    }
            | OutputStmt ';'    {   $$ = $1;    }
            | AsgStmt ';'       {   $$ = $1;    }
            | Ifstmt ';'        {   $$ = $1;    }
            | Whilestmt ';'     {   $$ = $1;    }
            | RepeatStmt ';'    {   $$ = $1;    }
            | DoWhileStmt ';'   {   $$ = $1;    }
            | BreakStmt ';'     {   $$ = $1;    }
            | ContinueStmt ';'  {   $$ = $1;    }
            ;

BreakStmt   : BREAK             {   $$ = make_break_node(); }
            ;

ContinueStmt    : CONTINUE      {   $$ = make_continue_node();  }  
                ;

InputStmt   : READ'('ID')'  {
                                $$ = make_operator_node(TYPE_NONE,NODE_READ,$3,NULL);
                            }
            ;

OutputStmt  : WRITE'('E')'  {  
                                $$ = make_operator_node(TYPE_NONE,NODE_WRITE,$3,NULL);
                            }
            ;

Ifstmt  : IF '(' E ')' THEN Slist ELSE Slist ENDIF  {   
                                                        if($3->type != TYPE_BOOL){
                                                            fprintf(stderr,"Error: Type Mismatch\n");
                                                            exit(1);
                                                        }
                                                        $$ = make_conditional_node($3,$6,$8);   
                                                    }
        | IF '(' E ')' THEN Slist ENDIF             {   
                                                        if($3->type != TYPE_BOOL){
                                                            fprintf(stderr,"Error: Type Mismatch\n");
                                                            exit(1);
                                                        }
                                                        $$ = make_conditional_node($3, $6, NULL);  
                                                    }
        ;

Whilestmt   : WHILE '(' E ')' DO Slist ENDWHILE     {      
                                                        if($3->type != TYPE_BOOL){
                                                            fprintf(stderr,"Error: Type Mismatch\n");
                                                            exit(1);
                                                        }
                                                        $$ = create_tree(0,TYPE_NONE,NULL,NODE_WHILE,$3,NULL,$6);
                                                    }
            ;
RepeatStmt  :  REPEAT Slist UNTIL '(' E ')'         {
                                                        if($5->type != TYPE_BOOL){
                                                            fprintf(stderr, "Error: Type Mismatch");
                                                            exit(1);
                                                        }
                                                        $$ = create_tree(0,TYPE_NONE,NULL,NODE_REPEAT,$2,NULL,$5);
                                                    }
DoWhileStmt : DO Slist WHILE '(' E ')'              {
                                                        if($5->type != TYPE_BOOL){
                                                            fprintf(stderr, "Error: Type Mismatch");
                                                            exit(1);
                                                        }
                                                        $$ = create_tree(0,TYPE_NONE,NULL,NODE_DOWHILE,$2,NULL,$5);
                                                    }

AsgStmt     : ID '=' E  {     
                            $$ = make_operator_node(TYPE_NONE, NODE_ASGN, $1, $3);
                        }
            ;
    
E   :   E '<' E     {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $$ = make_operator_node(TYPE_BOOL,NODE_LT,$1,$3);
                    }
    |   E '>' E     {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $$ = make_operator_node(TYPE_BOOL,NODE_GT,$1,$3);
                    }
    |   E '<''=' E  {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $$ = make_operator_node(TYPE_BOOL,NODE_LE,$1,$4);
                    }
    |   E '>''=' E  {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $$ = make_operator_node(TYPE_BOOL,NODE_GE,$1,$4);
                    }
    |   E '!''=' E  {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $$ = make_operator_node(TYPE_BOOL,NODE_NE,$1,$4);
                    }
    |   E '=''=' E  {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $$ = make_operator_node(TYPE_BOOL,NODE_EQ,$1,$4);
                    }
    |   E '+' E     {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $$ = make_operator_node(type,NODE_ADD,$1,$3);
                    }
    |   E '*' E     {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $$ = make_operator_node(type,NODE_MUL,$1,$3);
                    }
    |   E '/' E     {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $$ = make_operator_node(type,NODE_DIV,$1,$3);
                    }
    |   E '-' E     {
                        if($1->type != TYPE_INT || $3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $$ = make_operator_node(type,NODE_SUB,$1,$3);
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
    yyin = fopen(argv[2],"r");
    if(!yyin){
        perror("fopen");
        return 1;
    }
    output_file = fopen(argv[4],"w");
    if(!output_file){
        perror("fopen");
        return 1;
    }
    yyparse();
    return 1;
}