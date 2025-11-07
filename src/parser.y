%{
    #define YYERROR_CALL(msg) yyerror();
    #include <stdio.h>
    #include <stdlib.h>
    #include "node/type_node.h"
    #include "type_table/type_table.h"
    #include "node/ast_node.h"
    #include "node/decl_node.h"
    #include "code_gen/code_gen.h"
    #include "oops/class.h"
    int yyerror();   
    int yylex();
    extern FILE* yyin;
    FILE * output_file;
%}
%union 
{
    tnode * ast_node;   // for creating the tree node for code gen
    char * id_name;   // for getting the variable name for symbol tree creation
    Type* decl_type;      // for getting the type of the variable while declaration
    struct decl_node * decl_node;  // for declarations section to register variables to symbol table
    Lsymbol* lsymbol_entry;   // local symbol table entry
    parameter* param_list;  // list of parameters for the fns
    //// TYPE DEF
    char* type_name;    // for type name in struct fields
    FieldList* field_list;  // list of fields for type def
}
%token ID READ WRITE IF THEN ELSE ENDIF WHILE DO ENDWHILE BREAK CONTINUE REPEAT UNTIL INT STR DECL ENDDECL P_BEGIN P_END RETURN_STMT;
%token NUM_VAL STR_VAL;
%token MAIN_DEF;
%token AND OR NOT;
%token TYPE ENDTYPE;
%token INITIALIZE ALLOC FREE;
%token NULL_VAL;
%token TUPLE;
%token BREAK_POINT;
%token CLASS ENDCLASS EXTENDS SELF NEW DELETE;
%type<decl_type> Type PointerType;
%type<param_list> Param ParamList ParamListBracs;
%type<lsymbol_entry> LIdDecl LIdList LDecl LDeclList LDeclBlock;
%type<type_name> TypeName;
%type<field_list> FieldDecl FieldDeclList;
%nonassoc '<' '>' '=' ';' '&';
%left '+' '-';
%left '*' '/' '%';
%left '.';
%left '(' ')';
%%
///////////////                              Main program                                       /////////////////////////

Program     :   TypeDefBlock ClassDefBlock GDeclBlock FDefBlock MainBlock  {   
                                                    exit(0);
                                                }
            ;


///////////////                                     TYPE                                          /////////////////////

TypeDefBlock  : TYPE TypeDefList ENDTYPE    { // type table creation completed
                                                print_type_table();
                                            }
              |
              ;

TypeDefList   : TypeDefList TypeDef
              | TypeDef
              ;

TypeDef       : ID '{' FieldDeclList '}'    {   TypeTable* struct_def = type_table_add($<id_name>1,1,$3); // throws error if field size > 8
                                                // returns error if any non declared type is used in the fields here
                                                update_field_types(struct_def); 
                                                free($<id_name>1);
                                            }
              ;

FieldDeclList : FieldDeclList FieldDecl     {   $$ = field_list_join($1, $2);   }
              | FieldDecl                   {   $$ = $1;  }
              ;

FieldDecl    : TypeName ID ';'  {   TypeTable* temp_type = type_table_get($1);  
                                    if(temp_type == NULL){
                                        // could be the currently being parsed struct type
                                        temp_type = create_temp_type_table($1);
                                    }
                                    $$ = field_create($<id_name>2, temp_type);
                                }

TypeName     : INT      {   $$ = strdup("int");     }
             | STR      {   $$ = strdup("str");    } 
             | ID       {   $$ = $<id_name>1;       }//TypeName for user-defined types
             ;

////////////////                               Class Block                                        ///////////////////////

ClassDefBlock   :   CLASS ClassDefList ENDCLASS     {   code_gen_class_vft(output_file);    }
                |                                   {}
                ;

ClassDefList    :   ClassDefList ClassDef   {}
                |   ClassDef                {}
                ;

ClassDef        :   Cname '{' DECL FieldList MethodDecl ENDDECL MethodDef '}'   {
                                                                                    curr_class = NULL;
                                                                                }
                ;

Cname           :   ID              {   
                                        ClassTable* cptr = ct_install($<id_name>1, NULL); 
                                        curr_class = cptr;
                                    }
                |   ID EXTENDS ID   {   
                                        ClassTable* cptr = ct_install($<id_name>1, $<id_name>3); 
                                        // install parent fields and methods
                                        ct_install_inherited(cptr);
                                        curr_class = cptr;
                                    }
                ;

FieldList       :   FieldList FID   {}
                |
                ;

FID             :   Type ID ';'       {
                                        TypeTable* type = $1->type_table;
                                        ClassTable* c_type = $1->c_type;
                                        class_f_install(curr_class, c_type, type, $<id_name>2);
                                        free($1);

                                    }
                ;

MethodDecl      :   MethodDecl  MDecl   {}
                |   MDecl               {}
                |
                ;


MDecl           :   Type ID '(' ParamList ')' ';'     {
                                                        TypeTable* type = $1->type_table;
                                                        // ClassTable* c_type = ct_get($<id_name>1);
                                                        if(class_m_get(curr_class, $<id_name>2) != NULL){
                                                            // Method already declared, we are just redefining
                                                            if(class_m_get(curr_class->parent_ptr, $<id_name>2) == NULL){
                                                                // not inherited method, error
                                                                fprintf(stderr, "ERROR: Method redeclared:%s\n",$<id_name>2);
                                                                exit(1);
                                                            }
                                                            else{
                                                                MethodList* parent_method = class_m_get(curr_class, $<id_name>2);
                                                                // Check type and arglist
                                                                if(compare_type_table(parent_method->type, type) == 0){
                                                                    fprintf(stderr,"ERROR: Mismatching method definition:%s",$<id_name>2);
                                                                    exit(1);
                                                                }

                                                                if(same_parameter_list(parent_method->param_list, $4) == 0){
                                                                    fprintf(stderr,"ERROR: Mismatching method definition:%s",$<id_name>2);
                                                                    exit(1);
                                                                }

                                                                parent_method->f_label = get_f_label();
                                                            }
                                                        }
                                                        else{
                                                            class_m_install(curr_class, $<id_name>2, type, $4, get_f_label());
                                                        }

                                                        free($1);
                                                        
                                                    }
                ;   

MethodDef       :   MethodDef FDef              {}
                |   FDef                        {}
                ;

////////////////                                GDeclBlock                                        ///////////////////////

GDeclBlock  :   DECL GDeclList ENDDECL  {   
                                            print_st();
                                        }
            |   DECL ENDDECL            {}
            |   /*empty*/               {}
            ;

GDeclList   :   GDeclList GDecl {}
            |   GDecl           {}
            ;

GDecl       :   Type GIdList ';'    {   
                                        update_type_decl($<decl_node>2, $1);  
                                        // print_st(); 
                                        if(is_tuple($1)){
                                            update_size_decl($<decl_node>2, $1, output_file);
                                        }
                                        // print_st();
                                        free($1);
                                    }
            ;

GIdList     :   GIdList ',' GIdDecl     {   $<decl_node>$ = add_to_list($<decl_node>1, $<decl_node>3); }
            |   GIdDecl                 {   $<decl_node>$ = $<decl_node>1; }
            ;


GIdDecl      : ID '[' NUM_VAL ']'                {
                                                    tnode* ast_node = $<ast_node>3;
                                                    int sz = ast_node->val.int_val;
                                                    free(ast_node);
                                                    array* arr_sz = add_array_node(NULL, sz);
                                                    Type * dummy_type = create_type(type_table_get("int"), 0);
                                                    $<decl_node>$ = create_decl_node_arr($<id_name>1, sz, dummy_type, arr_sz, output_file);
                                                    free(dummy_type);
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
                                                    
                                                    Type * dummy_type = create_type(type_table_get("int"), 0);
                                                    $<decl_node>$ = create_decl_node_arr($<id_name>1, sz, dummy_type, arr_sz, output_file);
                                                    free(dummy_type);
                                                    free($<id_name>1);
                                                }

            | ID                {   
                                    Type * dummy_type = create_type(type_table_get("int"), 0);
                                    $<decl_node>$ = create_decl_node($<id_name>1,1,dummy_type, output_file);
                                    free(dummy_type);
                                    free($<id_name>1);                      
                                }
            | '*' ID            {   
                                    Type * dummy_type = create_type(type_table_get("int"), 1);
                                    $<decl_node>$ = create_decl_node($<id_name>2,1,dummy_type, output_file);
                                    free(dummy_type);
                                    free($<id_name>2);
                                }
            |   ID ParamListBracs       {   
                                            free_lsymbol(curr_lsymbol);
                                            curr_lsymbol = NULL;
                                            Type * dummy_type = create_type(type_table_get("int"), 0);
                                            $<decl_node>$ = create_decl_node_fn($<id_name>1, dummy_type, $2);
                                            free(dummy_type);
                                            free($<id_name>1);
                                        }
            |   '*' ID ParamListBracs   {   
                                            free_lsymbol(curr_lsymbol);
                                            curr_lsymbol = NULL;
                                            Type * dummy_type = create_type(type_table_get("int"), 1);
                                            $<decl_node>$ = create_decl_node_fn($<id_name>2, dummy_type, $3);
                                            free(dummy_type);
                                            free($<id_name>2);
                                        }
            ;
///////////////                                            LDeclBlock                                            ////////////////////

LDeclBlock      : DECL LDeclList ENDDECL    {   curr_lsymbol = $$ = connect_lsymbol(curr_lsymbol, $2);    }
                | DECL ENDDECL              {   $$ = curr_lsymbol; }
                | /*empty*/                 {   $$ = curr_lsymbol; }
                ;

LDeclList   : LDeclList LDecl     {     $$ = connect_lsymbol($1,$2);   }
            | LDecl               {     $$ = $1;  }
            ;

LDecl       : Type LIdList ';'  {   $$ = update_type_lsymbol_tb($2, $1); 
                                    if(is_tuple($1)){
                                        $$ = update_size_lsymbol_tb($2,$1);
                                    }
                                    free($1);          
                                }
            ;

PointerType : Type '*'      {   $1->ptr = 1;
                                $$ = $1;  }
            | Type          {   $$ = $1;  }
            ;

Type        : INT           {   TypeTable* type_table_entry = type_table_get("int");
                                if(type_table_entry == NULL){
                                    fprintf(stderr, "Undefined type used:int\n");
                                    exit(1);
                                }
                                $$ = create_type(type_table_entry,0);
                            }
            | STR           {   TypeTable* type_table_entry = type_table_get("str");
                                if(type_table_entry == NULL){
                                    fprintf(stderr, "Undefined type used: str\n");
                                    exit(1);
                                }
                                $$ = create_type(type_table_entry,0);
                            }
            | ID            {   TypeTable* type_table_entry = type_table_get($<id_name>1);
                                if(type_table_entry == NULL){
                                    // could be a class
                                    ClassTable* cptr = ct_get($<id_name>1);
                                    if(cptr){
                                        $$ = create_type_class(cptr);
                                    }
                                    else{
                                        fprintf(stderr, "Undefined type used:%s\n",$<id_name>1);
                                        exit(1);
                                    }
                                }
                                else{
                                    $$ = create_type(type_table_entry,0);
                                }
                            }
            | TUPLE ID '(' ParamList ')'   {
                                            char name[50];
                                            strcpy(name, "tuple-");
                                            strcat(name, $<id_name>2);
                                            // print_param_list($3);   // debug
                                            FieldList* fields = paraml_to_fieldl($4);
                                            TypeTable* tytbl = type_table_add(name, 1, fields);
                                            // print_type_table(); // debug
                                            // printf("[DEBUG]name:%s\n",name);
                                            $$ = create_type(tytbl, 0);
                                        }
            ;

LIdList     : LIdList ',' LIdDecl   {   
                                        $$ = connect_lsymbol($1,$3); // $1 is actually curr_lsymbol
                                    }
            | LIdDecl               {
                                        $$ = $1;
                                    }
            ;

LIdDecl     : ID                {
                                    Type * dummy_type = create_type(type_table_get("int"), 0);
                                    $$ = create_lsymbol($<id_name>1,dummy_type,lst_binding++,-1,NULL);
                                    free(dummy_type);
                                    free($<id_name>1);                      
                                }
            | '*' ID            {   
                                    Type * dummy_type = create_type(type_table_get("int"), 1);
                                    $$ = create_lsymbol($<id_name>2,dummy_type,lst_binding++,-1,NULL);
                                    free(dummy_type);
                                    free($<id_name>2);
                                }
            ;
/////////////                                       Fn Defintions                                             ///////////////
FDefBlock   :   FDefBlock FDef  {}
            |   FDef            {}
            |
            ;

FDef        :   PointerType ID ParamListBracs '{' LDeclBlock Body '}'   {
                                                                        Gsymbol* fn_decl = get_variable_gst($<id_name>2);
                                                                        MethodList* method = NULL;

                                                                        if(fn_decl){    // global fn def
                                                                            if(compare_type(fn_decl->type_entryy, $1) == 0){
                                                                                fprintf(stderr,"ERROR: Mismatching function definition:%s",$<id_name>2);
                                                                                exit(1);
                                                                            }
                                                                            free($1);
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
    
                                                                            code_gen_fn($<ast_node>6, output_file);
                                                                            free_tree($<ast_node>6);
                                                                            free_lsymbol(curr_lsymbol);
                                                                            curr_lsymbol = NULL;
                                                                            lst_binding = 1;

                                                                        }
                                                                        else{
                                                                            method = class_m_get(curr_class, $<id_name>2);
                                                                            if(method){ // in class
                                                                                if(compare_type_table(method->type, $1->type_table) == 0){
                                                                                    fprintf(stderr,"ERROR: Mismatching function definition:%s",$<id_name>2);
                                                                                    exit(1);
                                                                                }
                                                                                free($1);
                                                                                if(same_parameter_list(method->param_list,$3)!=1){
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
        
                                                                                fprintf(output_file,"_F%d:\n",method->f_label);
        
                                                                                code_gen_fn($<ast_node>6, output_file);
                                                                                free_tree($<ast_node>6);
                                                                                free_lsymbol(curr_lsymbol);
                                                                                curr_lsymbol = NULL;
                                                                                lst_binding = 1;
                                                          
                                                                            }
                                                                            else{
                                                                                fprintf(stderr,"ERROR: Function declaration not found:%s",$<id_name>2);
                                                                                exit(1);
                                                                            }
                                                                        }


                                                                    }
            ;
ParamListBracs  :   '(' ParamList ')'   {   
                                            curr_lsymbol = add_paramlist_lsymbol($2, NULL,-3);
                                            if(curr_class != NULL){
                                                //add self to local symbol table
                                                curr_lsymbol = add_self_lsymbol(curr_lsymbol, curr_class);
                                            }
                                            $$ = $2;    
                                        }
                ;

ParamList   :   ParamList ',' Param {   $$ = add_parameter_to_list($1, $3); }
            |   Param               {   $$ = $1;    }
            |   /* empty */         {   $$ = NULL;  }
            ;

Param       :   Type ID         { 
                                    $$ = create_parameter($<id_name>2, $1); 
                                    free($1);
                                }
            |   Type '*' ID     { 
                                    $1->ptr = 1;
                                    $$ = create_parameter($<id_name>3, $1); 
                                    free($1);
                                }
            ;

Body        :   P_BEGIN Slist P_END      {   $<ast_node>$ = $<ast_node>2;    }
            |   /* empty */ {   $<ast_node>$ = NULL;    }
            |   P_BEGIN P_END   {   $<ast_node>$ = NULL;    }
            ;


////////////                                   Main Block                                                       /////////////////

MainBlock   :   MAIN_DEF '(' ')' '{' LDeclBlock Body '}'    {
                                                                code_gen_SP_init(output_file);
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

Slist       : Slist Stmt    {   $<ast_node>$ = make_operator_node(NULL,NODE_CONN,$<ast_node>1,$<ast_node>2);   }
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
            | E ';'         {   $<ast_node>$ = $<ast_node>1;    }
            | BREAK_POINT ';'   { $<ast_node>$ = make_breakpoint_node();}
            ;

BreakStmt   : BREAK             {   $<ast_node>$ = make_break_node(); }
            ;

ContinueStmt    : CONTINUE      {   $<ast_node>$ = make_continue_node();  }  
                ;

InputStmt   : READ'('LHS')'  {
                                $<ast_node>$ = make_operator_node(NULL,NODE_READ,$<ast_node>3,NULL);
                            }
            ;

OutputStmt  : WRITE'('E')'  {  
                                $<ast_node>$ = make_operator_node(NULL,NODE_WRITE,$<ast_node>3,NULL);
                            }
            ;

Ifstmt  : IF '(' E ')' THEN Slist ELSE Slist ENDIF  {   
                                                        if(is_bool($<ast_node>3->type_entryy) != 1){
                                                            fprintf(stderr,"Error: Type Mismatch\n");
                                                            exit(1);
                                                        }
                                                        $<ast_node>$ = make_conditional_node($<ast_node>3,$<ast_node>6,$<ast_node>8);   
                                                    }
        | IF '(' E ')' THEN Slist ENDIF             {   
                                                        if(is_bool($<ast_node>3->type_entryy) != 1){
                                                            fprintf(stderr,"Error: Type Mismatch\n");
                                                            exit(1);
                                                        }
                                                        $<ast_node>$ = make_conditional_node($<ast_node>3, $<ast_node>6, NULL);  
                                                    }
        ;

Whilestmt   : WHILE '(' E ')' DO Slist ENDWHILE     {      
                                                        if(is_bool($<ast_node>3->type_entryy) != 1){
                                                            fprintf(stderr,"Error: Type Mismatch\n");
                                                            exit(1);
                                                        }
                                                        node_val val;
                                                        val.int_val = 0;
                                                        $<ast_node>$ = create_tree(val,NULL,NULL,NODE_WHILE,NULL,NULL,$<ast_node>3,NULL,$<ast_node>6,NULL);
                                                    }
            ;
RepeatStmt  :  REPEAT Slist UNTIL '(' E ')'         {
                                                        if(is_bool($<ast_node>3->type_entryy) != 1){
                                                            fprintf(stderr, "Error: Type Mismatch");
                                                            exit(1);
                                                        }
                                                        node_val val;
                                                        val.int_val = 0;
                                                        $<ast_node>$ = create_tree(val,NULL,NULL,NODE_REPEAT,NULL,NULL,$<ast_node>2,NULL,$<ast_node>5,NULL);
                                                    }
DoWhileStmt : DO Slist WHILE '(' E ')'              {
                                                        if(is_bool($<ast_node>3->type_entryy) != 1){
                                                            fprintf(stderr, "Error: Type Mismatch");
                                                            exit(1);
                                                        }
                                                        node_val val;
                                                        val.int_val = 0;
                                                        $<ast_node>$ = create_tree(val,NULL,NULL,NODE_DOWHILE,NULL,NULL,$<ast_node>2,NULL,$<ast_node>5,NULL);
                                                    }

ReturnStmt  :   RETURN_STMT E                       {
                                                        $<ast_node>$ = make_return_node($<ast_node>2);
                                                    }
            ;
L_VAL   :   SELF     { //class -- will always have LST entry
                        char* name = "self";
                        Lsymbol* lst_entry = get_variable_lst(name, curr_lsymbol);
                        $<ast_node>$ = make_self_node(curr_class, lst_entry);
                    }
        |   ID  {   // can be str or int - doesn't matter. Symbol table holds the binding to which value is added
                    node_val val;
                    val.int_val = 0;
                    Lsymbol* lst_entry = get_variable_lst($<id_name>1, curr_lsymbol);
                    Type* type = NULL;
                    Gsymbol* gst_entry = NULL;
                    if(lst_entry == NULL){
                        gst_entry = get_variable_gst($<id_name>1);
                    }

                    if(lst_entry == NULL && gst_entry == NULL){
                        fprintf(stderr,"Variable not declared cannot be used:%s\n",$<id_name>1);
                        exit(1);
                    }
                    if(lst_entry != NULL){
                        type = lst_entry->type_entryy;
                    }
                    else{
                        type = gst_entry->type_entryy;
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
                        Type* type = NULL;
                        if(lst_entry != NULL){
                            type = lst_entry->type_entryy;
                        }
                        else{
                            type = gst_entry->type_entryy;
                        }
                        tnode* id_node = make_leaf_node(val, type, $<id_name>1, gst_entry, lst_entry);

                        if(id_node->type_entryy->ptr != 1){
                            fprintf(stderr,"Error: Type Mismatch in array\n");
                            exit(1);
                        }

                        // type of the node is the type of the ID node
                        $<ast_node>$ = make_array_node(type, id_node, $<ast_node>2);
                        $<ast_node>$->type_entryy->ptr = 0;
                    }
        
        |   '*' E   {
                        if($<ast_node>2->type_entryy && ($<ast_node>2->type_entryy->ptr == 0)){
                            fprintf(stderr,"Error(ptr): Type Mismatch\n");
                            exit(1);
                        }
                        $<ast_node>$ = make_value_at_node($<ast_node>2);
                    }
        /* | Field     {   $<ast_node>$ = $<ast_node>1; } */
        ;

LHS   :   LHS  '.' ID   {   
                                tnode* l_node = $<ast_node>1;
                                FieldList* field;
                                if(!l_node->type_entryy) {
                                        fprintf(stderr,"Error[.]: Type Error\n");
                                        exit(1);
                                }

                                if(l_node->type_entryy->c_type){
                                    // field of class
                                    field = class_f_get(l_node->type_entryy->c_type, $<id_name>3);
                                }
                                else{
                                    if(!l_node->type_entryy->type_table){
                                        fprintf(stderr,"Error[.]: Type Error\n");
                                        exit(1);
                                    }
                                    field = field_list_get($<id_name>3, l_node->type_entryy->type_table);
                                }

                                if(field == NULL){
                                    fprintf(stderr,"Error[.]: Field %s does not exist\n",$<id_name>3);
                                    exit(1);
                                }
                                // field can be class
                                Type* type = NULL;
                                ClassTable* c_type = field->c_type;
                                if(!field->c_type){
                                    type = create_type(field->type,0);
                                }
                                else{
                                    type = create_type_class(c_type);
                                }
                                type->c_type = c_type;
                                node_val val;
                                val.int_val = 0;
                                tnode* r_node = make_leaf_node(val, type, $<id_name>3, NULL, NULL);
                                $<ast_node>$ = make_member_of_node(l_node, type, r_node);
                                free(type);   
                            }
        |   L_VAL '.' ID       {   
                                tnode* l_node = $<ast_node>1;
                                FieldList* field;
                                if(!l_node->type_entryy) {
                                    fprintf(stderr,"Error[.]: Type Error\n");
                                    exit(1);
                                }
                                if(l_node->type_entryy->c_type){
                                    // field of class
                                    field = class_f_get(l_node->type_entryy->c_type, $<id_name>3);
                                }
                                else{
                                    if(!l_node->type_entryy->type_table){
                                        fprintf(stderr,"Error[.]: Type Error\n");
                                        exit(1);
                                    }
                                    field = field_list_get($<id_name>3, l_node->type_entryy->type_table);
                                }

                                if(field == NULL){
                                    fprintf(stderr,"Error[.]: Field %s does not exist\n",$<id_name>3);
                                    exit(1);
                                }
                                // field can be class
                                Type* type = NULL;
                                ClassTable* c_type = field->c_type;
                                if(!field->c_type){
                                    type = create_type(field->type,0);
                                }
                                else{
                                    type = create_type_class(c_type);
                                }
                                type->c_type = c_type;
                                node_val val;
                                val.int_val = 0;
                                tnode* r_node = make_leaf_node(val, type, $<id_name>3, NULL, NULL);
                                $<ast_node>$ = make_member_of_node(l_node, type, r_node);
                                free(type);   
                            }
        | L_VAL             {   $<ast_node>$ = $<ast_node>1;    }
        | '(' LHS ')'       {   $<ast_node>$ = $<ast_node>2;    }
        /* |   ID INDEX '.' ID   {   // array
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
                        Type* type = NULL;
                        if(lst_entry != NULL){
                            type = lst_entry->type_entryy;
                        }
                        else{
                            type = gst_entry->type_entryy;
                        }
                        tnode* id_node = make_leaf_node(val, type, $<id_name>1, gst_entry, lst_entry);

                        if(id_node->type_entryy->ptr != 1){
                            fprintf(stderr,"Error: Type Mismatch in array\n");
                            exit(1);
                        }

                        // type of the node is the type of the ID node
                        tnode* l_node = make_array_node(type, id_node, $<ast_node>2);
                        l_node->type_entryy->ptr = 0;

                        if(!l_node->type_entryy || !l_node->type_entryy->type_table){
                                    fprintf(stderr,"Error[.]: Type Error\n");
                                    exit(1);
                        }
                        FieldList* field = field_list_get($<id_name>4, l_node->type_entryy->type_table);
                        if(field == NULL){
                            fprintf(stderr,"Error[.]: Field %s does not exist\n",$<id_name>4);
                            exit(1);
                        }
                        type = create_type(field->type,0);
                        val.int_val = 0;
                        tnode* r_node = make_leaf_node(val, type, $<id_name>4, NULL, NULL);
                        $<ast_node>$ = make_member_of_node(l_node, type, r_node);
                        free(type); 
                    } */
        ;

AsgStmt     : LHS '=' E  {    
                            if(!compare_type($<ast_node>1->type_entryy, $<ast_node>3->type_entryy) && !is_null($<ast_node>3->type_entryy)){
                                // if class, then extra check
                                if(!compare_class_type($<ast_node>1->type_entryy, $<ast_node>3->type_entryy)){
                                    fprintf(stderr,"Error[=]: Type Mismatch\n");
                                    exit(1);
                                }
                            } 
                            $<ast_node>$ = make_operator_node(NULL, NODE_ASGN, $<ast_node>1, $<ast_node>3);
                        }
            ;
    
E   :   E '<' E     {
                        if(!compare_type($<ast_node>1->type_entryy, $<ast_node>3->type_entryy) || !is_int($<ast_node>1->type_entryy)){
                            fprintf(stderr,"Error[<]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("bool"),0);
                        $<ast_node>$ = make_operator_node(type,NODE_LT,$<ast_node>1,$<ast_node>3);
                        free(type);
                    }
    |   E '>' E     {
                        if(!compare_type($<ast_node>1->type_entryy, $<ast_node>3->type_entryy) || !is_int($<ast_node>1->type_entryy)){
                            fprintf(stderr,"Error[>]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("bool"),0);
                        $<ast_node>$ = make_operator_node(type,NODE_GT,$<ast_node>1,$<ast_node>3);
                        free(type);
                    }
    |   E '<''=' E  {
                        if(!compare_type($<ast_node>1->type_entryy, $<ast_node>4->type_entryy) || !is_int($<ast_node>1->type_entryy)){
                            fprintf(stderr,"Error[<=]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("bool"),0);
                        $<ast_node>$ = make_operator_node(type,NODE_LE,$<ast_node>1,$<ast_node>4);
                        free(type);
                    }
    |   E '>''=' E  {
                        if(!compare_type($<ast_node>1->type_entryy, $<ast_node>4->type_entryy) || !is_int($<ast_node>1->type_entryy)){
                            fprintf(stderr,"Error[>=]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("bool"),0);
                        $<ast_node>$ = make_operator_node(type,NODE_GE,$<ast_node>1,$<ast_node>4);
                        free(type);
                    }
    |   E '!''=' E  {
                        if(
                            !is_null($<ast_node>1->type_entryy) &&
                            !is_null($<ast_node>4->type_entryy) && 
                            !compare_type($<ast_node>1->type_entryy, $<ast_node>4->type_entryy)
                        ){
                            fprintf(stderr,"Error[!=]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("bool"),0);
                        $<ast_node>$ = make_operator_node(type,NODE_NE,$<ast_node>1,$<ast_node>4);
                        free(type);
                    }
    |   E '=''=' E  {
                        if(
                            !is_null($<ast_node>1->type_entryy) &&
                            !is_null($<ast_node>4->type_entryy) && 
                            !compare_type($<ast_node>1->type_entryy, $<ast_node>4->type_entryy)
                        ){
                            fprintf(stderr,"Error[==]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("bool"),0);
                        $<ast_node>$ = make_operator_node(type,NODE_EQ,$<ast_node>1,$<ast_node>4);
                        free(type);
                    }
    |   E AND E     {
                        if(!compare_type($<ast_node>1->type_entryy, $<ast_node>3->type_entryy) || !is_bool($<ast_node>1->type_entryy)){
                            fprintf(stderr,"Error[AND]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("bool"),0);
                        $<ast_node>$ = make_operator_node(type,NODE_AND,$<ast_node>1,$<ast_node>3);
                        free(type);
                    }
    |   E OR E      {
                        if(!compare_type($<ast_node>1->type_entryy, $<ast_node>3->type_entryy) || !is_bool($<ast_node>1->type_entryy)){
                            fprintf(stderr,"Error[OR]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("bool"),0);
                        $<ast_node>$ = make_operator_node(type,NODE_OR,$<ast_node>1,$<ast_node>3);
                        free(type);
                    }
    |   NOT E       {
                        if(!is_bool($<ast_node>2->type_entryy)){
                            fprintf(stderr,"Error[NOT]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("bool"),0);
                        $<ast_node>$ = make_operator_node(type,NODE_NOT,$<ast_node>2,NULL);
                        free(type);
                    }
    |   E '+' E     {
                        if( !(
                            (is_int($<ast_node>1->type_entryy) && is_int($<ast_node>3->type_entryy)) ||
                            (is_int($<ast_node>1->type_entryy) && $<ast_node>3->type_entryy->ptr) ||
                            ($<ast_node>1->type_entryy->ptr && is_int($<ast_node>3->type_entryy))
                        )){
                            fprintf(stderr,"Error[+]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = NULL;
                        if($<ast_node>1->type_entryy && $<ast_node>1->type_entryy->ptr)
                            type = create_type($<ast_node>1->type_entryy->type_table,$<ast_node>1->type_entryy->ptr);
                        else if($<ast_node>3->type_entryy && $<ast_node>3->type_entryy->ptr)
                            type = create_type($<ast_node>3->type_entryy->type_table,$<ast_node>3->type_entryy->ptr);
                        if(!type) // is NULL
                            type = create_type(type_table_get("int"), 0);
                        $<ast_node>$ = make_operator_node(type,NODE_ADD,$<ast_node>1,$<ast_node>3);
                        free(type);
                    }
    |   E '%' E     {
                        if(!(is_int($<ast_node>1->type_entryy) && is_int($<ast_node>3->type_entryy))){
                            fprintf(stderr,"Error[%]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("int"), 0);
                        $<ast_node>$ = make_operator_node(type,NODE_MOD,$<ast_node>1,$<ast_node>3);
                        free(type);
                    }
    |   E '*' E     {
                        if(!(is_int($<ast_node>1->type_entryy) && is_int($<ast_node>3->type_entryy))){
                            fprintf(stderr,"Error[*]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("int"), 0);
                        $<ast_node>$ = make_operator_node(type,NODE_MUL,$<ast_node>1,$<ast_node>3);
                        free(type);
                    }
    |   E '/' E     {
                        if(!(is_int($<ast_node>1->type_entryy) && is_int($<ast_node>3->type_entryy))){
                            fprintf(stderr,"Error[/]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type(type_table_get("int"), 0);
                        $<ast_node>$ = make_operator_node(type,NODE_DIV,$<ast_node>1,$<ast_node>3);
                        free(type);
                    }
    |   E '-' E     {
                        if( !(
                            (is_int($<ast_node>1->type_entryy) && is_int($<ast_node>3->type_entryy)) ||
                            ($<ast_node>1->type_entryy->ptr && is_int($<ast_node>3->type_entryy))
                        )){
                            fprintf(stderr,"Error[-]: Type Mismatch\n");
                            exit(1);
                        }
                        Type* type = create_type($<ast_node>1->type_entryy->type_table, $<ast_node>1->type_entryy->ptr);
                        $<ast_node>$ = make_operator_node(type,NODE_SUB,$<ast_node>1,$<ast_node>3);
                        free(type);
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
    |   LHS         {   $<ast_node>$ = $<ast_node>1;    
                    }
    |   '&' E       {
                        $<ast_node>$ = make_address_of_node($<ast_node>2);
                    }
    |   FnCall      {   $<ast_node>$ = $<ast_node>1; }
    |   FieldFn     {   $<ast_node>$ = $<ast_node>1; }
    |   MemFn       {   $<ast_node>$ = $<ast_node>1; }
    | NULL_VAL      {   $<ast_node>$ = make_null_node(); }
    | NEW '(' ID ')'    {   
                            ClassTable* cptr = ct_get($<id_name>3);    
                            if(cptr == NULL){
                                fprintf(stderr, "[ERROR] Class not found:%s\n",$<id_name>3);
                                exit(1);
                            }
                            $<ast_node>$ = make_new_node(cptr);
                        }
    | DELETE '(' LHS ')'    {
                                // lhs should have type as class
                                ClassTable* cptr = $<ast_node>3->type_entryy->c_type;
                                if(cptr == NULL){
                                    fprintf(stderr, "[ERROR] Class not found:%s\n",$<id_name>3);
                                    exit(1);
                                }
                                $<ast_node>$ = make_del_node($<ast_node>3);
                            }
    ;

FieldFn :   L_VAL '.' ID '(' ArgList ')'  { // LHS can be SELF, ID, ID.ID -- definitely fn
                                            MethodList* fn = class_m_get($<ast_node>1->type_entryy->c_type, $<id_name>3);
                                            if(fn == NULL){
                                                printf("No declaration found for fn: %s",$<id_name>3);
                                                exit(1);
                                            }

                                            if(compare_arg_param($<ast_node>5, fn->param_list) == 0){
                                                printf("Mismatching type for function:%s\n",$<id_name>3);
                                                exit(1);
                                            }
                                            Type* type = create_type(fn->type, 0);
                                            $<ast_node>$ = make_method_of_node($<ast_node>1, $<id_name>3, type, $<ast_node>5);

                                        }
        |   LHS '.' ID '(' ArgList ')'  { // LHS can be SELF, ID, ID.ID -- definitely fn
                                            MethodList* fn = class_m_get($<ast_node>1->type_entryy->c_type, $<id_name>3);
                                            if(fn == NULL){
                                                printf("No declaration found for fn: %s",$<id_name>3);
                                                exit(1);
                                            }

                                            if(compare_arg_param($<ast_node>5, fn->param_list) == 0){
                                                printf("Mismatching type for function:%s\n",$<id_name>3);
                                                exit(1);
                                            }
                                            Type* type = create_type(fn->type, 0);
                                            $<ast_node>$ = make_method_of_node($<ast_node>1, $<id_name>3, type, $<ast_node>5);

                                        }
        ;

MemFn   :   INITIALIZE '('')'   {   $<ast_node>$ = make_initialize_node(); }
        |   ALLOC '(' E ')'     {   if(!is_int($<ast_node>3->type_entryy)){
                                        printf("Error: Alloc() arg1 should be integer\n");
                                        exit(1);
                                    } 
                                    $<ast_node>$ = make_alloc_node();
                                }
        |   ALLOC '('  ')'      {
                                    $<ast_node>$ = make_alloc_node();
                                }
        |   FREE '(' E ')'      {   if(!is_int($<ast_node>3->type_entryy)){
                                        printf("Error: Free() arg1 should be integer\n");
                                        exit(1);
                                    } 
                                    $<ast_node>$ = make_free_node($<ast_node>3);
                                }
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
                                if(!is_int($<ast_node>2->type_entryy)){
                                    fprintf(stderr,"Error: Array index should be integer\n");
                                    exit(1);
                                }
                                $<ast_node>$ = make_index_node($<ast_node>3, $<ast_node>1);

                            }
        |   '[' E ']'   {
                            if(!is_int($<ast_node>2->type_entryy)){
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

    // initialise type table entries
    type_table = NULL;
    type_table_init();

    code_gen_start(output_file);
    fprintf(output_file, "MOV SP, %d\n", SP-1);
    fprintf(output_file, "JMP _F0\n");
    yyparse();
    return 1;
}