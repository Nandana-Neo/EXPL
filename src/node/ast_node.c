#include "ast_node.h"


NodeType node_type(char* type){
    if(type == NULL)
        return NODE_LEAF;
    switch(*type){
        case '+': return NODE_ADD;
        case '-': return NODE_SUB;
        case '*': return NODE_MUL;
        case '/': return NODE_DIV;
        case '=': return NODE_ASGN;
        case 'R': return NODE_READ;
        case 'W': return NODE_WRITE;
        case 'C': return NODE_CONN;
    }
    return NODE_CONN;
}

tnode* create_tree(node_val val, VarType type, char* varname, NodeType nodetype, tnode *l, tnode *m, tnode *r){
    tnode * curr = (tnode*)malloc(sizeof(tnode));
    // curr->val = val;
    curr->type = type;
    if(type == TYPE_INT)
        curr->val.int_val = val.int_val;
    else if(type == TYPE_STR)
        curr->val.str_val = val.str_val;
    else
        curr->val.int_val = 0;
    if(nodetype == NODE_LEAF){
        // leaf node
        curr->varname = varname;
        curr->nodetype = NODE_LEAF;
        curr->left = curr->right = NULL;
        return curr;
    }
    //else
    curr->varname = NULL;
    curr->left = l;
    curr->middle = m;
    curr->right = r;
    curr->nodetype = nodetype;
    return curr;
}

tnode* make_leaf_node(node_val n, VarType type, char *varname){
    tnode* curr = create_tree(n,type,varname,NODE_LEAF,NULL,NULL,NULL);
    return curr;
}

tnode* make_operator_node(VarType type, NodeType nodetype,tnode* l, tnode* r){
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val,type,NULL,nodetype,l,NULL,r);
    return curr;
}

tnode* make_break_node(void){
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val, TYPE_NONE, NULL, NODE_BREAK, NULL, NULL, NULL);
    return curr;
}

tnode* make_continue_node(void){
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val, TYPE_NONE, NULL, NODE_CONTINUE, NULL, NULL, NULL);
    return curr;
}

tnode* make_conditional_node(tnode* l, tnode* m, tnode* r){
    NodeType nodetype = NODE_IFELSE;
    if(m == NULL)
        nodetype = NODE_IF;
    node_val val;
    val.int_val = 0;
    tnode* curr = create_tree(val,TYPE_NONE,NULL,nodetype,l,m,r);
    return curr;
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