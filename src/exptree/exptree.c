#include "exptree.h"


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

tnode* create_tree(int val, VarType type, char* varname, NodeType nodetype, tnode *l, tnode *m, tnode *r){
    tnode * curr = (tnode*)malloc(sizeof(tnode));
    curr->val = val;
    curr->type = type;
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

tnode* make_leaf_node(int n, VarType type, char *varname){
    tnode* curr = create_tree(n,type,varname,NODE_LEAF,NULL,NULL,NULL);
    return curr;
}

tnode* make_operator_node(VarType type, NodeType nodetype,tnode* l, tnode* r){
    tnode* curr = create_tree(0,type,NULL,nodetype,l,NULL,r);
    return curr;
}

tnode* make_conditional_node(tnode* l, tnode* m, tnode* r){
    NodeType nodetype = NODE_IFELSE;
    if(m == NULL)
        nodetype = NODE_IF;
    tnode* curr = create_tree(0,TYPE_NONE,NULL,nodetype,l,m,r);
    return curr;
}

void prefix(tnode* node){
    if(node == NULL){
        return;
    }
    if(node->type == TYPE_NONE)
        printf("NODE(%d) ",node->nodetype);
    else{
        if(node->varname == NULL)
            printf("%d ",node->val);
        else
            printf("%c ",*node->varname);

    }
    
    prefix(node->left);
    prefix(node->right);
}