#include "exptree.h"


NodeType node_type(char* type){
    if(type == NULL)
        return NODE_LEAF;
    switch(*type){
        case '+': return NODE_ADD;
        case '-': return NODE_SUB;
        case '*': return NODE_MUL;
        case '/': return NODE_DIV;
        case '=': return NODE_EQ;
        case 'R': return NODE_READ;
        case 'W': return NODE_WRITE;
        case 'C': return NODE_CONN;
    }
    return NODE_CONN;
}

tnode* create_tree(int val, VarType type, char* c, tnode *l, tnode *r){
    tnode * curr = (tnode*)malloc(sizeof(tnode));
    curr->val = val;
    curr->type = type;
    if(type != TYPE_NONE){
        // leaf node
        curr->varname = c;
        curr->nodetype = NODE_LEAF;
        curr->left = curr->right = NULL;
        return curr;
    }
    //else
    curr->varname = NULL;
    curr->left = l;
    curr->right = r;
    curr->nodetype = node_type(c);
    return curr;
}

tnode* make_leaf_node(int n, VarType type, char *varname){
    tnode * curr = create_tree(n,type,varname,NULL,NULL);
    return curr;
}

tnode* make_operator_node(char c,tnode* l, tnode* r){
    tnode* curr = create_tree(0,TYPE_NONE,&c,l,r);
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