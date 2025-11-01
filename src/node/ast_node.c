#include "ast_node.h"


NodeType node_type(char* type){
    if(type == NULL)
        return NODE_LEAF;
    switch(*type){
        case '+': return NODE_ADD;
        case '-': return NODE_SUB;
        case '*': return NODE_MUL;
        case '/': return NODE_DIV;
        case '%': return NODE_MOD;
        case '=': return NODE_ASGN;
        case 'R': return NODE_READ;
        case 'W': return NODE_WRITE;
        case 'C': return NODE_CONN;
    }
    return NODE_CONN;
}

tnode* create_tree(node_val val, Type* type, char* varname, NodeType nodetype, Gsymbol *gst_entry, Lsymbol* lst_entry, tnode *l, tnode *m, tnode *r, tnode* nxt){
    tnode * curr = (tnode*)malloc(sizeof(tnode));
    if(type){
        if(type->c_type)
            curr->type_entryy = create_type_class(type->c_type);
        else
            curr->type_entryy = create_type(type->type_table, type->ptr);
    }
    else
        curr->type_entryy = NULL;
    if(is_int(type) == 1)
        curr->val.int_val = val.int_val;
    else if(is_str(type) == 1)
        curr->val.str_val = val.str_val;
    else
        curr->val.int_val = 0;
    curr->gst_entry = gst_entry;
    curr->lst_entry = lst_entry;
    curr->left = l;
    curr->middle = m;
    curr->right = r;
    curr->next = nxt;
    curr->nodetype = nodetype;
    if(nodetype == NODE_LEAF){
        // leaf node
        curr->varname = varname;
        curr->nodetype = NODE_LEAF;
        return curr;
    }
    //else
    curr->varname = NULL; // only exception is a fn
    return curr;
}

tnode* make_leaf_node(node_val n, Type* type, char* varname, Gsymbol* gst_entry, Lsymbol* lst_entry){
    tnode* curr = create_tree(n,type,varname,NODE_LEAF,gst_entry,lst_entry,NULL,NULL,NULL,NULL);
    return curr;
}

tnode* make_operator_node(Type* type, NodeType nodetype,tnode* l, tnode* r){
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val,type,NULL,nodetype,NULL,NULL,l,NULL,r,NULL);
    return curr;
}

tnode* make_break_node(void){
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val, NULL, NULL, NODE_BREAK, NULL, NULL, NULL, NULL, NULL, NULL);
    return curr;
}

tnode* make_continue_node(void){
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val, NULL, NULL, NODE_CONTINUE, NULL, NULL,NULL, NULL, NULL, NULL);
    return curr;
}

tnode* make_self_node(ClassTable* cptr, Lsymbol* lst_entry){
    node_val val;
    val.int_val = 0;
    Type* type = create_type_class(cptr);
    tnode* curr = create_tree(val, type, NULL, NODE_SELF, NULL, lst_entry, NULL, NULL, NULL, NULL);
    free(type);
    // curr->c_type = cptr;
    return curr;
}

tnode* make_conditional_node(tnode* l, tnode* m, tnode* r){
    NodeType nodetype = NODE_IFELSE;
    if(m == NULL)
        nodetype = NODE_IF;
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val,NULL,NULL,nodetype,NULL,NULL,l,m,r,NULL);
    return curr;
}

tnode* make_array_node(Type* type, tnode* l, tnode* r){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, type, NULL, NODE_ARR, NULL, NULL, l, NULL, r, NULL);
    return node;
}

tnode* make_index_node(tnode* l, tnode* r){
    node_val val;
    val.int_val = 0;
    Type* type = create_type(type_table_get("int") ,0);
    tnode* node = create_tree(val, type, NULL, NODE_INDEX, NULL, NULL, l, NULL, r, NULL);
    free(type);
    return node;
}

tnode* make_address_of_node(tnode* e){
    node_val val;
    val.int_val = 0;
    Type* type = create_type(e->type_entryy->type_table, 1);     // pointer type
    tnode* node = create_tree(val, type, NULL, NODE_ADDR_OF, NULL, NULL, e, NULL, NULL, NULL);
    free(type);
    return node;
}


tnode* make_value_at_node(tnode* e){
    node_val val;
    val.int_val = 0;
    Type* type = create_type(e->type_entryy->type_table, 0);
    tnode* node = create_tree(val, type, NULL, NODE_VAL_AT, NULL, NULL, e, NULL, NULL, NULL);
    free(type);
    return node;
}

tnode* make_member_of_node(tnode* parent, Type* type, tnode* child){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, type, NULL, NODE_MEMBER_OF, NULL, NULL, parent, NULL, child, NULL);
    return node;
}


tnode* make_method_of_node(tnode* parent, char* fn_name, Type* type, tnode* arg){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, type, fn_name, NODE_METHOD_OF, NULL, NULL, parent, arg, NULL, NULL);
    node->varname = strdup(fn_name);
    return node;
}

tnode* make_fn_node(char* name, tnode* arg){
    node_val val;
    val.int_val = 0;
    Gsymbol* gst_entry = get_variable_gst(name);
    if(gst_entry == NULL){
        printf("No declaration found for fn: %s",name);
        exit(1);
    }
    tnode* node = create_tree(val, gst_entry->type_entryy, name, NODE_FN, gst_entry, NULL, arg, NULL, NULL, NULL);
    node->varname = name;
    return node;
}

tnode* make_return_node(tnode* e){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, e->type_entryy, NULL, NODE_RET, NULL, NULL, e, NULL, NULL, NULL);
    return node;
}

tnode* make_null_node() {
    node_val val;
    val.int_val = 0;
    Type* type = create_type(type_table_get("null"),0);
    tnode* node = create_tree(val, type, NULL, NODE_NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

tnode* make_initialize_node(){
    node_val val;
    val.int_val = 0;
    Type* type = create_type(type_table_get("int"), 0);
    tnode* node = create_tree(val, type, NULL, NODE_INITIALIZE, NULL, NULL, NULL, NULL, NULL, NULL);
    free(type);
    return node;
}

tnode* make_alloc_node(){
    node_val val;
    val.int_val = 0;
    Type* type = create_type(type_table_get("null"),0); // null pointer first
    tnode* node = create_tree(val, type, NULL, NODE_ALLOC, NULL, NULL, NULL, NULL, NULL, NULL);
    free(type);
    return node;
}

tnode* make_free_node(tnode* ptr){
    node_val val;
    val.int_val = 0;
    Type* type = create_type(type_table_get("int"),0);
    tnode* node = create_tree(val, type, NULL, NODE_FREE, NULL, NULL, ptr, NULL, NULL, NULL);
    free(type);
    return node;
}


tnode* make_new_node(ClassTable* c){
    node_val val;
    val.int_val = 0;
    Type* type = create_type_class(c);
    tnode* node = create_tree(val, type, NULL, NODE_NEW, NULL, NULL, NULL, NULL, NULL, NULL);
    free(type);
    return node;
}

tnode* make_del_node(tnode* id){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, NULL, NULL, NODE_DEL, NULL, NULL, id, NULL, NULL, NULL);
    return node;
}

tnode* make_breakpoint_node(){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, NULL, NULL, NODE_BREAK_POINT, NULL, NULL, NULL, NULL, NULL, NULL);
    return node;
}

void prefix(tnode* node){
    if(node == NULL){
        return;
    }
    if(node->type_entryy == NULL)
        printf("NODE(%d) ",node->nodetype);
    else{
        if(node->varname == NULL){
            if(is_int(node->type_entryy))
                printf("%d ",node->val.int_val);
            else if(is_str(node->type_entryy))
                printf("%s ",node->val.str_val);
        }
        else
            printf("%c ",*node->varname);

    }
    
    prefix(node->left);
    prefix(node->right);
}


tnode* add_node_to_arglist(tnode* arg, tnode* node){
    if (arg == NULL) {
        return node;
    }
    
    if (arg == NULL) {
        return node;
    }

    // Traverse to the last node
    tnode* current = arg;
    while (current->next != NULL) {
        current = current->next;
    }
    
    // Attach the new node
    current->next = node;
    return arg;
}

int compare_arg_param(tnode* arg_ls, parameter* param_ls){
    if(arg_ls == NULL && param_ls == NULL)
        return 1;
    if(arg_ls==NULL || param_ls == NULL)
        return 0;
    if(compare_type(arg_ls->type_entryy, param_ls->type_entryy) != 1)
        return 0;
    return compare_arg_param(arg_ls->next, param_ls->next);
}

void print_arg_ls(tnode* node){
    if(node==NULL)
        return;
    if(node->type_entryy == NULL)
        printf("NULL\n");
    else
        printf("Arg:%s\n",node->type_entryy->type_table->name);
    print_arg_ls(node->next);
}

void free_tree(tnode* n){
    if(n==NULL)
        return;
    free_tree(n->left);
    free_tree(n->middle);
    free_tree(n->right);
    if(n->varname != NULL)
        free(n->varname);
    // if(n->nodetype == NODE_LEAF && is_str(n->type_entryy))
    //     free(n->val.str_val);
    if(n->type_entryy)
        free(n->type_entryy);
    free(n);
}