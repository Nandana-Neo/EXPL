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

tnode* create_tree(node_val val, VarType type, char* varname, NodeType nodetype, Gsymbol *gst_entry, Lsymbol* lst_entry, tnode *l, tnode *m, tnode *r, tnode* nxt){
    tnode * curr = (tnode*)malloc(sizeof(tnode));
    curr->type = type;
    if(type == TYPE_INT)
        curr->val.int_val = val.int_val;
    else if(type == TYPE_STR)
        curr->val.str_val = val.str_val;
    else
        curr->val.int_val = 0;
    curr->gst_entry = gst_entry;
    curr->lst_entry = lst_entry;
    if(nodetype == NODE_LEAF){
        // leaf node
        curr->varname = varname;
        curr->nodetype = NODE_LEAF;
        curr->left = curr->right = NULL;
        return curr;
    }
    //else
    curr->varname = NULL; // only exception is a fn
    curr->left = l;
    curr->middle = m;
    curr->right = r;
    curr->next = nxt;
    curr->nodetype = nodetype;
    return curr;
}

tnode* make_leaf_node(node_val n, VarType type, char* varname, Gsymbol* gst_entry, Lsymbol* lst_entry){
    tnode* curr = create_tree(n,type,varname,NODE_LEAF,gst_entry,lst_entry,NULL,NULL,NULL,NULL);
    return curr;
}

tnode* make_operator_node(VarType type, NodeType nodetype,tnode* l, tnode* r){
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val,type,NULL,nodetype,NULL,NULL,l,NULL,r,NULL);
    return curr;
}

tnode* make_break_node(void){
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val, TYPE_NONE, NULL, NODE_BREAK, NULL, NULL, NULL, NULL, NULL, NULL);
    return curr;
}

tnode* make_continue_node(void){
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val, TYPE_NONE, NULL, NODE_CONTINUE, NULL, NULL,NULL, NULL, NULL, NULL);
    return curr;
}

tnode* make_conditional_node(tnode* l, tnode* m, tnode* r){
    NodeType nodetype = NODE_IFELSE;
    if(m == NULL)
        nodetype = NODE_IF;
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val,TYPE_NONE,NULL,nodetype,NULL,NULL,l,m,r,NULL);
    return curr;
}

tnode* make_array_node(VarType type, tnode* l, tnode* r){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, type, NULL, NODE_ARR, NULL, NULL, l, NULL, r, NULL);
    return node;
}

tnode* make_index_node(tnode* l, tnode* r){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, TYPE_INT, NULL, NODE_INDEX, NULL, NULL, l, NULL, r, NULL);
    return node;
}

tnode* make_address_of_node(tnode* e){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, pointer_type(e->type), NULL, NODE_ADDR_OF, NULL, NULL, e, NULL, NULL, NULL);
    return node;
}

tnode* make_value_at_node(tnode* e){
    node_val val;
    val.int_val = 0;
    tnode* node = create_tree(val, variable_type(e->type), NULL, NODE_VAL_AT, NULL, NULL, e, NULL, NULL, NULL);
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
    tnode* node = create_tree(val, gst_entry->type, name, NODE_FN, gst_entry, NULL, arg, NULL, NULL, NULL);
    node->varname = name;
    return node;
}

void prefix(tnode* node){
    if(node == NULL){
        return;
    }
    if(node->type == TYPE_NONE)
        printf("NODE(%d) ",node->nodetype);
    else{
        if(node->varname == NULL){
            if(node->type == TYPE_INT)
                printf("%d ",node->val.int_val);
            else if(node->type == TYPE_STR)
                printf("%s ",node->val.str_val);
        }
        else
            printf("%c ",*node->varname);

    }
    
    prefix(node->left);
    prefix(node->right);
}


tnode* add_node_to_arglist(tnode* arg, tnode* node){
    if(arg == NULL)
        return node;
    if(node == NULL)
        return arg;
    arg->next = add_node_to_arglist(arg->next, node);
    return arg;
}

int compare_arg_param(tnode* arg_ls, parameter* param_ls){
    if(arg_ls == NULL && param_ls == NULL)
        return 1;
    if(arg_ls->type != param_ls->type)
        return 0;
    return compare_arg_param(arg_ls->next, param_ls->next);
}

void free_tree(tnode* n){
    if(n==NULL)
        return;
    free_tree(n->left);
    free_tree(n->middle);
    free_tree(n->right);
    if(n->varname != NULL)
        free(n->varname);
    if(n->nodetype == NODE_LEAF && n->type == TYPE_STR)
        free(n->val.str_val);
    free(n);
}