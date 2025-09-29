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
%token ID READ WRITE IF THEN ELSE ENDIF WHILE DO ENDWHILE BREAK CONTINUE REPEAT UNTIL INT STR DECL ENDDECL P_BEGIN P_END RETURN_STMT;
%token NUM_VAL STR_VAL;
%token MAIN_DEF;
%token AND OR NOT;
%type<decl_type> Type;
%type<param_list> Param ParamList ParamListBracs;
%type<lsymbol_entry> LIdDecl LIdList LDecl LDeclList LDeclBlock;
%nonassoc '<' '>' '=' ';' '&';
%left '+' '-';
%left '*' '/' '%';
%%
Program     :   GDeclBlock FDefBlock MainBlock  {   
                                                    // evaluate($<ast_node>3);
                                                    exit(0);
                                                }
            |   GDeclBlock MainBlock            {      
                                                    // evaluate($<ast_node>3);
                                                    exit(0);
                                                }
            ;

/* Program     : GDeclBlock P_BEGIN Slist P_END   {
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
            ; */


GDeclBlock  :   DECL GDeclList ENDDECL  {   
                                            code_gen_SP_init(output_file);
                                            fprintf(output_file, "JMP _F0\n");
                                        }
            |   DECL ENDDECL            {   code_gen_SP_init(output_file);
                                            fprintf(output_file, "JMP _F0\n");
                                        }
            |   /*empty*/               {   code_gen_SP_init(output_file);
                                            fprintf(output_file, "JMP _F0\n");
                                        }
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
            |   ID ParamListBracs       {   
                                            free_lsymbol(curr_lsymbol);
                                            curr_lsymbol = NULL;
                                            $<decl_node>$ = create_decl_node_fn($<id_name>1, TYPE_INT, $2);
                                            free($<id_name>1);
                                        }
            ;
///////////////////////////////////////////////////////////////////////////////////////
LDeclBlock      : DECL LDeclList ENDDECL    {   curr_lsymbol = $$ = connect_lsymbol(curr_lsymbol, $2);    }
                | DECL ENDDECL              {   $$ = curr_lsymbol; }
                | /*empty*/                 {   $$ = curr_lsymbol; }
                ;

LDeclList   : LDeclList LDecl     {     $$ = connect_lsymbol($1,$2);   }
            | LDecl               {     $$ = $1;  }
            ;

LDecl       : Type LIdList ';'  {   $$ = update_type_lsymbol_tb($2, $1); }
            ;

Type        : INT           {   $$ = TYPE_INT; }
            | STR           {   $$ = TYPE_STR; }
            ;

LIdList     : LIdList ',' LIdDecl   {   
                                        $$ = connect_lsymbol($1,$3); // $1 is actually curr_lsymbol
                                    }
            | LIdDecl               {
                                        $$ = $1;
                                    }
            ;

LIdDecl     : ID                {
                                    $$ = create_lsymbol($<id_name>1,TYPE_INT,lst_binding++,NULL);
                                    free($<id_name>1);                      
                                }
            | '*' ID            {   
                                    $$ = create_lsymbol($<id_name>2,TYPE_INT_PTR,lst_binding++,NULL);
                                    free($<id_name>2);
                                }
            ;
///////////////////////////////////////////////////////////////////////////////////////////////
FDefBlock   :   FDefBlock FDef  {}
            |   FDef            {}
            ;

FDef        :   Type ID ParamListBracs '{' LDeclBlock Body '}'   {
                                                                        Gsymbol* fn_decl = get_variable_gst($<id_name>2);
                                                                        if(fn_decl == NULL){
                                                                            fprintf(stderr,"ERROR: Function declaration not found:%s",$<id_name>2);
                                                                            exit(1);
                                                                        }
                                                                        if(fn_decl->type != $1){
                                                                            fprintf(stderr,"ERROR: Mismatching function definition:%s",$<id_name>2);
                                                                            exit(1);
                                                                        }
                                                                        if(same_parameter_list(fn_decl->param_list,$3)!=1){
                                                                            fprintf(stderr,"ERROR: Mismatching function definition:%s",$<id_name>2);
                                                                            exit(1);
                                                                        }
                                                                        if(curr_lsymbol != $5){
                                                                            fprintf(stderr,"ERROR in implementation of fn:%s",$<id_name>2);
                                                                            exit(1);
                                                                        }
                                                                        Lsymbol* repeated_node = lst_if_repeated(curr_lsymbol);
                                                                        if(repeated_node){
                                                                            fprintf(stderr,"Variable redeclared:%s\n",repeated_node->varname);
                                                                            exit(1);
                                                                        }
                                                                        free_param_list($3);
                                                                        print_lsymbol();

                                                                        fprintf(output_file,"_F%d:\n",fn_decl->f_label);

                                                                        code_gen_fn($<ast_node>6, output_file); // TODO
                                                                        free_tree($<ast_node>6);
                                                                        free_lsymbol(curr_lsymbol);
                                                                        curr_lsymbol = NULL;
                                                                        lst_binding = 1;
                                                                    }
            ;
ParamListBracs  :   '(' ParamList ')'   {   
                                            curr_lsymbol = add_paramlist_lsymbol($2, NULL,-3);
                                            $$ = $2;    
                                        }
                ;

ParamList   :   ParamList ',' Param {   $$ = add_parameter_to_list($1, $3); }
            |   Param               {   $$ = $1;    }
            |   /* empty */         {   $$ = NULL;  }
            ;

Param       :   Type ID     { $$ = create_parameter($<id_name>2, $1); }
            |   Type '*' ID    { $$ = create_parameter($<id_name>3, pointer_type($1)); }
            ;

Body        :   P_BEGIN Slist P_END      {   $<ast_node>$ = $<ast_node>2;    }
            |   /* empty */ {   $<ast_node>$ = NULL;    }
            ;
///////////////////////////////////////////////////////////////////////////////////////
MainBlock   :   MAIN_DEF '(' ')' '{' LDeclBlock Body '}'    {
                                                                if(curr_lsymbol != $5){
                                                                    fprintf(stderr,"ERROR in implementation of fn: main");
                                                                    exit(1);
                                                                }
                                                                Lsymbol* repeated_node = lst_if_repeated(curr_lsymbol);
                                                                if(repeated_node){
                                                                    fprintf(stderr,"Variable redeclared:%s\n",repeated_node->varname);
                                                                    exit(1);
                                                                }
                                                                print_lsymbol();

                                                                fprintf(output_file, "_F0:\n");
                                                                fprintf(output_file, "PUSH R0\n");  // return value for main
                                                                fprintf(output_file, "MOV R0, _L0\n");  // Push return address 
                                                                fprintf(output_file, "PUSH R0\n"); 
                                                                code_gen_fn_begin(output_file);
                                                                code_gen($<ast_node>6, output_file, -1, -1);
                                                                code_gen_final(output_file);

                                                                free_tree($<ast_node>6);
                                                                free_lsymbol(curr_lsymbol);
                                                                curr_lsymbol = NULL;
                                                                lst_binding = 1;
                                                            }
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
            | ReturnStmt ';'    {   $<ast_node>$ = $<ast_node>1;    }
            | FnCall ';'    {   $<ast_node>$ = $<ast_node>1;    }
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
                                                        $<ast_node>$ = create_tree(val,TYPE_NONE,NULL,NODE_WHILE,NULL,NULL,$<ast_node>3,NULL,$<ast_node>6,NULL);
                                                    }
            ;
RepeatStmt  :  REPEAT Slist UNTIL '(' E ')'         {
                                                        if($<ast_node>5->type != TYPE_BOOL){
                                                            fprintf(stderr, "Error: Type Mismatch");
                                                            exit(1);
                                                        }
                                                        node_val val;
                                                        val.int_val = 0;
                                                        $<ast_node>$ = create_tree(val,TYPE_NONE,NULL,NODE_REPEAT,NULL,NULL,$<ast_node>2,NULL,$<ast_node>5,NULL);
                                                    }
DoWhileStmt : DO Slist WHILE '(' E ')'              {
                                                        if($<ast_node>5->type != TYPE_BOOL){
                                                            fprintf(stderr, "Error: Type Mismatch");
                                                            exit(1);
                                                        }
                                                        node_val val;
                                                        val.int_val = 0;
                                                        $<ast_node>$ = create_tree(val,TYPE_NONE,NULL,NODE_DOWHILE,NULL,NULL,$<ast_node>2,NULL,$<ast_node>5,NULL);
                                                    }

ReturnStmt  :   RETURN_STMT E                       {
                                                        $<ast_node>$ = make_return_node($<ast_node>2);
                                                    }
            ;
L_VAL   :   ID  {   // can be str or int - doesn't matter. Symbol table holds the binding to which value is added
                    node_val val;
                    val.int_val = 0;
                    Lsymbol* lst_entry = get_variable_lst($<id_name>1, curr_lsymbol);
                    VarType type = TYPE_INT;
                    Gsymbol* gst_entry = NULL;
                    if(lst_entry == NULL){
                        gst_entry = get_variable_gst($<id_name>1);
                    }

                    if(lst_entry == NULL && gst_entry == NULL){
                        fprintf(stderr,"Variable not declared cannot be used:%s\n",$<id_name>1);
                        exit(1);
                    }
                    if(lst_entry != NULL){
                        type = lst_entry->type;
                    }
                    else{
                        type = gst_entry->type;
                    }
                    $<ast_node>$ = make_leaf_node(val, type, $<id_name>1, gst_entry, lst_entry);
                }
        
        |   ID INDEX{   // array
                        node_val val;
                        val.int_val = 0;
                        Lsymbol* lst_entry = get_variable_lst($<id_name>1, curr_lsymbol);
                        Gsymbol* gst_entry = NULL;
                        if(lst_entry == NULL)
                            gst_entry = get_variable_gst($<id_name>1);

                        if(lst_entry == NULL && gst_entry == NULL){
                            fprintf(stderr,"Variable not declared cannot be used:%s\n",$<id_name>1);
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        if(lst_entry != NULL){
                            type = lst_entry->type;
                        }
                        else{
                            type = gst_entry->type;
                        }
                        tnode* id_node = make_leaf_node(val, type, $<id_name>1, gst_entry, lst_entry);

                        if(id_node->type != TYPE_INT_PTR && id_node->type != TYPE_CHAR_PTR){
                            fprintf(stderr,"Error: Type Mismatch in array\n");
                            exit(1);
                        }

                        // type of the node is the type of the ID node
                        $<ast_node>$ = make_array_node(variable_type(type), id_node, $<ast_node>2);
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
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>4->type != TYPE_INT){
                            fprintf(stderr,"Error[<=]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_LE,$<ast_node>1,$<ast_node>4);
                    }
    |   E '>''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>4->type != TYPE_INT){
                            fprintf(stderr,"Error[>=]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_GE,$<ast_node>1,$<ast_node>4);
                    }
    |   E '!''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>4->type != TYPE_INT){
                            fprintf(stderr,"Error[!=]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_NE,$<ast_node>1,$<ast_node>4);
                    }
    |   E '=''=' E  {
                        if($<ast_node>1->type != TYPE_INT || $<ast_node>4->type != TYPE_INT){
                            fprintf(stderr,"Error[==]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_EQ,$<ast_node>1,$<ast_node>4);
                    }
    |   E AND E     {
                        if($<ast_node>1->type != TYPE_BOOL || $<ast_node>3->type != TYPE_BOOL){
                            fprintf(stderr,"Error[AND]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_AND,$<ast_node>1,$<ast_node>3);
                    }
    |   E OR E      {
                        if($<ast_node>1->type != TYPE_BOOL || $<ast_node>3->type != TYPE_BOOL){
                            fprintf(stderr,"Error[OR]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_OR,$<ast_node>1,$<ast_node>3);
                    }
    |   NOT E       {
                        if($<ast_node>2->type != TYPE_BOOL){
                            fprintf(stderr,"Error[NOT]: Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_operator_node(TYPE_BOOL,NODE_NOT,$<ast_node>2,NULL);
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
                        if((!is_pointer_type($<ast_node>1->type) && $<ast_node>1->type != TYPE_INT) || (!is_pointer_type($<ast_node>3->type) && $<ast_node>3->type != TYPE_INT )){
                            fprintf(stderr,"Error[-]: Type Mismatch\n");
                            exit(1);
                        }
                        VarType type = TYPE_INT;
                        if(is_pointer_type($<ast_node>1->type))
                            type = $<ast_node>1->type;
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
    | FnCall        {   $<ast_node>$ = $<ast_node>1; }
    ;

FnCall  :   ID '(' ArgList ')'  {   
                                    // printf("\n\n[DEBUG]Fn Call:%s\n",$<id_name>1);                                        
                                    Gsymbol* fn_decl = get_variable_gst($<id_name>1);
                                    if(fn_decl == NULL){
                                        printf("No declaration found for fn: %s",$<id_name>1);
                                        exit(1);
                                    }

                                    // printf("[DEBUG]: PARAMLIST:\n");
                                    // print_param_list(fn_decl->param_list); // DEBUG
                                    // printf("[DEBUG]: ARGLIST:\n");
                                    // print_arg_ls($<ast_node>3);             // DEBUG

                                    if(compare_arg_param($<ast_node>3, fn_decl->param_list) == 0){
                                        printf("Mismatching type for function:%s\n",$<id_name>1);
                                        exit(1);
                                    }
                                    // Type checking on ArgList
                                    $<ast_node>$ = make_fn_node($<id_name>1, $<ast_node>3);
                                }
        ;


ArgList :   ArgList ',' E   {   $<ast_node>$ = add_node_to_arglist($<ast_node>1, $<ast_node>3);
                                tnode* node = $<ast_node>$; }
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
    curr_lsymbol = NULL;
    lst_binding = 1;
    code_gen_start(output_file);
    yyparse();
    return 1;
}