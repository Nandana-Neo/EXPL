%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "node/type_node.h"
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
    Lsymbol* lsymbol_entry;   // local symbol table entry
    parameter* param_list;  // list of parameters for the fns
}
%token ID P_BEGIN P_END READ WRITE IF THEN ELSE ENDIF WHILE DO ENDWHILE BREAK CONTINUE REPEAT UNTIL INT STR DECL ENDDECL;
%token NUM_VAL STR_VAL;
%token MAIN_DEF;
%type<decl_type> Type;
%type<param_list> Param ParamList;
%nonassoc '<' '>' '=' ';' '&';
%left '+' '-';
%left '*' '/' '%';
%%
/* Program     :   GDeclBlock FDefBlock MainBlock  {}
            |   GDeclBlock MainBlock            {}
            |   MainBlock                       {}
            ; */

Program     : GDeclBlock P_BEGIN Slist P_END   {
                                    FILE * fp = output_file;
                                    code_gen_SP_init(fp);
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


GDeclBlock  :   DECL GDeclList ENDDECL  {}
            |   DECL ENDDECL            {}
            ;

GDeclList   :   GDeclList GDecl {}
            |   GDecl           {}
            ;

GDecl       :   Type GIdList ';'    {   update_type_decl($<decl_node>2, $1);   }
            ;

GIdList     :   GIdList ',' GIdDecl     {   $<decl_node>$ = add_to_list($<decl_node>1, $<decl_node>3); }
            |   GIdDecl                 {   $<decl_node>$ = $<decl_node>1; }
            ;


GIdDecl      : ID '[' NUM_VAL ']'                {
                                                    tnode* ast_node = $<ast_node>3;
                                                    int sz = ast_node->val.int_val;
                                                    free(ast_node);
                                                    array* arr_sz = add_array_node(NULL, sz);
                                                    $<decl_node>$ = create_decl_node_arr($<id_name>1, sz, TYPE_INT, arr_sz, output_file);
                                                    free($<id_name>1);
                                                }
            | ID '[' NUM_VAL ']' '[' NUM_VAL ']'{
                                                    tnode* ast_node_r = $<ast_node>3;
                                                    int sz1 = ast_node_r->val.int_val;
                                                    tnode* ast_node_c = $<ast_node>6;
                                                    int sz2 = ast_node_c->val.int_val;
                                                    int sz = sz1*sz2;
                                                    free(ast_node_c);
                                                    free(ast_node_r);
                                                    array* arr_sz = add_array_node(NULL, sz1);
                                                    arr_sz = add_array_node(arr_sz, sz2);
                                                    
                                                    $<decl_node>$ = create_decl_node_arr($<id_name>1, sz, TYPE_INT, arr_sz, output_file);
                                                    free($<id_name>1);
                                                }

            | ID                {   $<decl_node>$ = create_decl_node($<id_name>1,1,TYPE_INT);
                                    free($<id_name>1);                      
                                }
            | '*' ID            {   
                                    $<decl_node>$ = create_decl_node($<id_name>2,1,TYPE_INT_PTR);
                                    free($<id_name>2);
                                }
            |   ID '(' ParamList ')'    {   
                                            $<decl_node>$ = create_decl_node_fn($<id_name>1, TYPE_INT, $3);
                                            free($<id_name>1);
                                        }
            ;
///////////////////////////////////////////////////////////////////////////////////////
LDeclBlock      : DECL LDeclList ENDDECL    {   $<lsymbol_entry>$ = curr_lsymbol = $<lsymbol_entry>2; }
                | DECL ENDDECL              {   $<lsymbol_entry>$ = NULL; }
                ;

LDeclList   : LDeclList LDecl     {     $<lsymbol_entry>$ = connect_lsymbol($<lsymbol_entry>1,$<lsymbol_entry>2);   }
            | LDecl               {     $<lsymbol_entry>$ = $<lsymbol_entry>1;  }
            ;

LDecl       : Type LIdList ';'  {   $<lsymbol_entry>$ = update_type_lsymbol_tb($<lsymbol_entry>2, $1); }
            ;

Type        : INT           {   $$ = TYPE_INT; }
            | STR           {   $$ = TYPE_STR; }
            ;

LIdList     : LIdList ',' LIdDecl   {
                                        $<lsymbol_entry>$ = connect_lsymbol($<lsymbol_entry>1,$<lsymbol_entry>3);
                                    }
            | LIdDecl               {
                                        $<lsymbol_entry>$ = $<lsymbol_entry>1;
                                    }
            ;

LIdDecl     : ID                {   
                                    $<lsymbol_entry>$ = create_lsymbol($<id_name>1,TYPE_INT,-1,NULL);
                                    free($<id_name>1);                      
                                }
            | '*' ID            {   
                                    $<lsymbol_entry>$ = create_lsymbol($<id_name>2,TYPE_INT_PTR,-1,NULL);
                                    free($<id_name>2);
                                }
            ;
///////////////////////////////////////////////////////////////////////////////////////////////
FDefBlock   :   FDefBlock FDef  {}
            |   FDef            {}
            ;

FDef        :   Type ID '(' ParamList ')' '{' LDeclBlock Body '}'   {
                                                                        Gsymbol* fn_decl = get_variable_gst($<id_name>2);
                                                                        if(fn_decl->type != $1){
                                                                            fprintf(stderr,"Mismatching function definition:%s",$<id_name>2);
                                                                            exit(1);
                                                                        }
                                                                        if(same_parameter_list(fn_decl->param_list,$4)!=1){
                                                                            fprintf(stderr,"Mismatching function definition:%s",$<id_name>2);
                                                                            exit(1);
                                                                        }
                                                                        Lsymbol* lsymbol_table_entry = $<lsymbol_entry>7;
                                                                        lsymbol_table_entry = add_paramlist_lsymbol($4, lsymbol_table_entry);
                                                                        free_param_list($4);
                                                                        fprintf(output_file,"_F%d:\n",fn_decl->flabel);
                                                                        curr_lsymbol = lsymbol_table_entry;
                                                                        
                                                                        code_gen($<ast_node>8, fp, -1, -1); // TODO
                                                                        fprintf("RET\n");
                                                                        free_tree($<ast_node>8);
                                                                        free_lsymbol(lsymbol_table_entry);
                                                                        
                                                                    }
            ;

ParamList   :   ParamList ',' Param {   $$ = add_parameter_to_list($1, $3); }
            |   Param               {   $$ = $1;    }
            |   /* empty */         {   $$ = NULL;  }
            ;

Param       :   Type ID     { $$ = create_parameter($<id_name>2, $1); }
            ;

Body        :   Slist       {   $<ast_node>$ = $<ast_node>1;    }
            |   /* empty */ {   $<ast_node>$ = NULL;    }
            ;
///////////////////////////////////////////////////////////////////////////////////////
MainBlock   :   MAIN_DEF '(' ')' '{' LDeclBlock Body '}'    {}
            ;

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
                                $<ast_node>$ = make_operator_node(TYPE_NONE,NODE_READ,$<ast_node>3,NULL);
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

L_VAL   :   ID  {   // can be str or int - doesn't matter. Symbol table holds the binding to which value is added
                    node_val val;
                    val.int_val = 0;
                    Gsymbol * st_entry = get_variable_gst($<id_name>1);
                    if(st_entry==NULL){
                        fprintf(stderr,"Variable not declared cannot be used:%s\n",$<id_name>1);
                        exit(1);
                    }
                    $<ast_node>$ = make_leaf_node(val,st_entry->type,$<id_name>1,st_entry);
                }
        
        |   ID INDEX{   // array
                        node_val val;
                        val.int_val = 0;
                        Gsymbol * st_entry = get_variable_gst($<id_name>1);
                        if(st_entry==NULL){
                            fprintf(stderr,"Variable not declared cannot be used:%s\n",$<id_name>1);
                            exit(1);
                        }
                        tnode* id_node = make_leaf_node(val,st_entry->type,$<id_name>1,st_entry);
                        if(id_node->type != TYPE_INT_PTR && id_node->type != TYPE_CHAR_PTR){
                            fprintf(stderr,"Error: Type Mismatch in array\n");
                            exit(1);
                        }

                        // type of the node is the type of the ID node
                        $<ast_node>$ = make_array_node(variable_type(id_node->type), id_node, $<ast_node>2);
                    }
        
        |   '*' E   {
                        if(!is_pointer_type($<ast_node>2->type)){
                            fprintf(stderr,"Error(ptr): Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_value_at_node($<ast_node>2);
                    }
        ;

AsgStmt     : L_VAL '=' E  {    
                            if($<ast_node>1->type != $<ast_node>3->type){
                                fprintf(stderr,"Error[=]: Type Mismatch\n");
                                exit(1);
                            } 
                            $<ast_node>$ = make_operator_node(TYPE_NONE, NODE_ASGN, $<ast_node>1, $<ast_node>3);
                        }
            ;
    
E   :   E '<' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[<]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_LT,$<ast_node>1,$<ast_node>3);
                    }
    |   E '>' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[>]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_GT,$<ast_node>1,$<ast_node>3);
                    }
    |   E '<''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[<=]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_LE,$<ast_node>1,$<ast_node>4);
                    }
    |   E '>''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[>=]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_GE,$<ast_node>1,$<ast_node>4);
                    }
    |   E '!''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[!=]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_NE,$<ast_node>1,$<ast_node>4);
                    }
    |   E '=''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[==]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_EQ,$<ast_node>1,$<ast_node>4);
                    }
    |   E '+' E     {
                        if((!is_pointer_type($<ast_node>1->type) && $<ast_node>1->type != TYPE_INT) || (!is_pointer_type($<ast_node>3->type) && $<ast_node>3->type != TYPE_INT )){
                            fprintf(stderr,"Error[+]: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        if(is_pointer_type($<ast_node>1->type))
                            type = $<ast_node>1->type;
                        else if(is_pointer_type($<ast_node>3->type))
                            type = $<ast_node>3->type;
                        $<ast_node>$ = make_operator_node(type,NODE_ADD,$<ast_node>1,$<ast_node>3);
                    }
    |   E '%' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[%]: Type Mismatch: T1: %d, T2: %d\n",$<ast_node>1->type, $<ast_node>3->type);
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $<ast_node>$ = make_operator_node(type,NODE_MOD,$<ast_node>1,$<ast_node>3);
                    }
    |   E '*' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[*]: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $<ast_node>$ = make_operator_node(type,NODE_MUL,$<ast_node>1,$<ast_node>3);
                    }
    |   E '/' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[/]: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $<ast_node>$ = make_operator_node(type,NODE_DIV,$<ast_node>1,$<ast_node>3);
                    }
    |   E '-' E     {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>3->type != TYPE_INT){
                            fprintf(stderr,"Error[-]: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        $<ast_node>$ = make_operator_node(type,NODE_SUB,$<ast_node>1,$<ast_node>3);
                    }
    |   '(' E ')'   {
                        $<ast_node>$ = $<ast_node>2;
                    }
    |   NUM_VAL     {
                        $<ast_node>$ = $<ast_node>1;
                    } 
    |   STR_VAL     {
                        $<ast_node>$ = $<ast_node>1;
                    }
    |   L_VAL       {   $<ast_node>$ = $<ast_node>1;    
                    }
    |   '&' E       {
                        $<ast_node>$ = make_address_of_node($<ast_node>2);
                    }
    |   ID '(' ArgList ')'  {   
                                                                        
                                Gsymbol* fn_decl = get_variable_gst($<id_name>1);
                                if(fn_decl == NULL){
                                    printf("No declaration found for fn: %s",$<id_name>1);
                                    exit(1);
                                }
                                if(compare_arg_param($<ast_node>3, fn_decl->param_list) == 0){
                                    printf("Mismatching type for function:%s",$<id_name>1);
                                    exit(1);
                                }
                                // Type checking on ArgList
                                $<ast_node>$ = make_fn_node($<id_name>1, $<ast_node>3);
                            }
    ;

ArgList :   ArgList ',' E   {   $<ast_node>$ = add_node_to_arglist($<ast_node>1, $<ast_node>3); }
        |   E               {   $<ast_node>$ = $<ast_node>1; }
        |   /* empty */     {   $<ast_node>$ = NULL ;}
        ;

INDEX   :   INDEX '[' E ']' {
                                if($<ast_node>3->type != TYPE_INT){
                                    fprintf(stderr,"Error: Array index should be integer\n");
                                    exit(1);
                                }
                                $<ast_node>$ = make_index_node($<ast_node>3, $<ast_node>1);

                            }
        |   '[' E ']'   {
                            if($<ast_node>2->type != TYPE_INT){
                                fprintf(stderr,"Error: Array index should be integer\n");
                                exit(1);
                            }
                            $<ast_node>$ = make_index_node($<ast_node>2, NULL);
                        }
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
    code_gen_start(output_file);
    yyparse();
    return 1;
}