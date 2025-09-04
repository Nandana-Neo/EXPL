%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "node/ast_node.h"
    #include "node/decl_node.h"
    #include "code_gen/code_gen.h"
    int yyerror();   
    int yylex();
    extern FILE* yyin;
    FILE * output_file;
%}
%union 
{
    tnode * ast_node;   // for creating the tree node for code gen
    char * id_name;   // for getting the variable name for symbol tree creation
    int decl_type;      // for getting the type of the variable while declaration
    struct decl_node * decl_node;  // for declarations section to register variables to symbol table
}
%token ID P_BEGIN P_END READ WRITE IF THEN ELSE ENDIF WHILE DO ENDWHILE BREAK CONTINUE REPEAT UNTIL INT STR DECL ENDDECL;
%token <ast_node> NUM_VAL STR_VAL;
%type <ast_node> L_VAL;
%left '+' '-';
%left '*' '/';
%nonassoc '<' '>' '=' ';';
%%
Program     : Declarations P_BEGIN Slist P_END   {
                                    FILE * fp = output_file;
                                    code_gen_start(fp);
                                    code_gen($<ast_node>3, fp, -1, -1);
                                    code_gen_final(fp);
                                    // evaluate($<ast_node>3);
                                    exit(0);
                                }
            | P_BEGIN P_END     {   
                                fprintf(stdout,"Empty program");
                                exit(0);
                            }
            ;


Declarations    : DECL DeclList ENDDECL     { print_st();}
                | DECL ENDDECL              {}
                ;


DeclList    : DeclList Decl      {}
            | Decl               {}
            ;


Decl        : Type VarList ';'  {   create_entries($<decl_node>2, $<decl_type>1); }
            ;


Type        : INT           {   $<decl_type>$ = TYPE_INT; }
            | STR           {   $<decl_type>$ = TYPE_STR; }
            ;


VarList     : VarList ',' ID '[' NUM_VAL ']'    {
                                                    tnode* ast_node = $<ast_node>5;
                                                    int sz = ast_node->val.int_val;
                                                    free(ast_node);
                                                    decl_node* array_node = create_decl_node($<id_name>3,sz);
                                                    $<decl_node>$ = add_to_list($<decl_node>1,array_node);
                                                }
            | ID '[' NUM_VAL ']'                {
                                                    tnode* ast_node = $<ast_node>3;
                                                    int sz = ast_node->val.int_val;
                                                    free(ast_node);
                                                    $<decl_node>$ = create_decl_node($<id_name>1,sz);
                                                }
            | VarList ',' ID    {   
                                    decl_node* id_node = create_decl_node($<id_name>3,1);
                                    $<decl_node>$ = add_to_list($<decl_node>1, id_node); 
                                }

            | ID                {   $<decl_node>$ = create_decl_node($<id_name>1,1); }
            ;


///////////////////////////////////////////////////////////////////////////////////////////////
Slist       : Slist Stmt    {   $<ast_node>$ = make_operator_node(TYPE_NONE,NODE_CONN,$<ast_node>1,$<ast_node>2);   }
            | Stmt          {   $<ast_node>$ = $<ast_node>1;    }
            ;

Stmt        : InputStmt ';'     {   $<ast_node>$ = $<ast_node>1;    }
            | OutputStmt ';'    {   $<ast_node>$ = $<ast_node>1;    }
            | AsgStmt ';'       {   $<ast_node>$ = $<ast_node>1;    }
            | Ifstmt ';'        {   $<ast_node>$ = $<ast_node>1;    }
            | Whilestmt ';'     {   $<ast_node>$ = $<ast_node>1;    }
            | RepeatStmt ';'    {   $<ast_node>$ = $<ast_node>1;    }
            | DoWhileStmt ';'   {   $<ast_node>$ = $<ast_node>1;    }
            | BreakStmt ';'     {   $<ast_node>$ = $<ast_node>1;    }
            | ContinueStmt ';'  {   $<ast_node>$ = $<ast_node>1;    }
            ;

BreakStmt   : BREAK             {   $<ast_node>$ = make_break_node(); }
            ;

ContinueStmt    : CONTINUE      {   $<ast_node>$ = make_continue_node();  }  
                ;

InputStmt   : READ'('L_VAL')'  {
                                $<ast_node>$ = make_operator_node(TYPE_NONE,NODE_READ,$3,NULL);
                            }
            ;

OutputStmt  : WRITE'('E')'  {  
                                $<ast_node>$ = make_operator_node(TYPE_NONE,NODE_WRITE,$<ast_node>3,NULL);
                            }
            ;

Ifstmt  : IF '(' E ')' THEN Slist ELSE Slist ENDIF  {   
                                                        if($<ast_node>3->type != TYPE_BOOL){
                                                            fprintf(stderr,"Error: Type Mismatch\n");
                                                            exit(1);
                                                        }
                                                        $<ast_node>$ = make_conditional_node($<ast_node>3,$<ast_node>6,$<ast_node>8);   
                                                    }
        | IF '(' E ')' THEN Slist ENDIF             {   
                                                        if($<ast_node>3->type != TYPE_BOOL){
                                                            fprintf(stderr,"Error: Type Mismatch\n");
                                                            exit(1);
                                                        }
                                                        $<ast_node>$ = make_conditional_node($<ast_node>3, $<ast_node>6, NULL);  
                                                    }
        ;

Whilestmt   : WHILE '(' E ')' DO Slist ENDWHILE     {      
                                                        if($<ast_node>3->type != TYPE_BOOL){
                                                            fprintf(stderr,"Error: Type Mismatch\n");
                                                            exit(1);
                                                        }
                                                        node_val val;
                                                        val.int_val = 0;
                                                        $<ast_node>$ = create_tree(val,TYPE_NONE,NULL,NODE_WHILE,NULL,$<ast_node>3,NULL,$<ast_node>6);
                                                    }
            ;
RepeatStmt  :  REPEAT Slist UNTIL '(' E ')'         {
                                                        if($<ast_node>5->type != TYPE_BOOL){
                                                            fprintf(stderr, "Error: Type Mismatch");
                                                            exit(1);
                                                        }
                                                        node_val val;
                                                        val.int_val = 0;
                                                        $<ast_node>$ = create_tree(val,TYPE_NONE,NULL,NODE_REPEAT,NULL,$<ast_node>2,NULL,$<ast_node>5);
                                                    }
DoWhileStmt : DO Slist WHILE '(' E ')'              {
                                                        if($<ast_node>5->type != TYPE_BOOL){
                                                            fprintf(stderr, "Error: Type Mismatch");
                                                            exit(1);
                                                        }
                                                        node_val val;
                                                        val.int_val = 0;
                                                        $<ast_node>$ = create_tree(val,TYPE_NONE,NULL,NODE_DOWHILE,NULL,$<ast_node>2,NULL,$<ast_node>5);
                                                    }

AsgStmt     : L_VAL '=' E  {    
                            if($1->type != $<ast_node>3->type){
                                fprintf(stderr,"Error: Type Mismatch\n");
                                exit(1);
                            } 
                            $<ast_node>$ = make_operator_node(TYPE_NONE, NODE_ASGN, $<ast_node>1, $<ast_node>3);
                        }
            ;
    
E   :   E '<' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_LT,$<ast_node>1,$<ast_node>3);
                    }
    |   E '>' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_GT,$<ast_node>1,$<ast_node>3);
                    }
    |   E '<''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_LE,$<ast_node>1,$<ast_node>4);
                    }
    |   E '>''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_GE,$<ast_node>1,$<ast_node>4);
                    }
    |   E '!''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_NE,$<ast_node>1,$<ast_node>4);
                    }
    |   E '=''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_EQ,$<ast_node>1,$<ast_node>4);
                    }
    |   E '+' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $<ast_node>$ = make_operator_node(type,NODE_ADD,$<ast_node>1,$<ast_node>3);
                    }
    |   E '*' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $<ast_node>$ = make_operator_node(type,NODE_MUL,$<ast_node>1,$<ast_node>3);
                    }
    |   E '/' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $<ast_node>$ = make_operator_node(type,NODE_DIV,$<ast_node>1,$<ast_node>3);
                    }
    |   E '-' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $<ast_node>$ = make_operator_node(type,NODE_SUB,$<ast_node>1,$<ast_node>3);
                    }
    |   '(' E ')'   {
                        $<ast_node>$ = $<ast_node>2;
                    }
    |   NUM_VAL     {
                        $<ast_node>$ = $1;
                    } 
    |   STR_VAL     {
                        $<ast_node>$ = $1;
                    }
    |   L_VAL       {   $<ast_node>$=$1;    }
    ;

L_VAL   :   ID  {   // can be str or int - doesn't matter. Symbol table holds the binding to which value is added
                    node_val val;
                    val.int_val = 0;
                    Gsymbol * st_entry = get_variable($<id_name>1);
                    if(st_entry==NULL){
                        fprintf(stderr,"Variable not declared cannot be used:%s\n",$<id_name>1);
                        exit(1);
                    }
                    $$ = make_leaf_node(val,st_entry->type,$<id_name>1,st_entry);
                }
        
        |   ID '[' E ']'{   // array
                        node_val val;
                        val.int_val = 0;
                        Gsymbol * st_entry = get_variable($<id_name>1);
                        if(st_entry==NULL){
                            fprintf(stderr,"Variable not declared cannot be used:%s\n",$<id_name>1);
                            exit(1);
                        }
                        tnode* id_node = make_leaf_node(val,st_entry->type,$<id_name>1,st_entry);
                        if(id_node->type == TYPE_NONE || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error: Type Mismatch in array\n");
                            exit(1);
                        }
                        // type of the node is the type of the ID node
                        $$ = make_array_node(id_node->type, id_node, $<ast_node>3);
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