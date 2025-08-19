#include "exptree.h"
#include <stdlib.h>
#include <stdio.h>

/*Make a leaf tnode and set the value of val field*/
tnode* makeLeafNode(int n){
    tnode * curr = (tnode *)malloc(sizeof(tnode));
    curr->val = n;
    curr->op = NULL;
    curr->left = curr->right = NULL;
    return curr;
}

/*Make a tnode with operator, left and right branches set*/
tnode* makeOperatorNode(char op, tnode *l, tnode *r){
    tnode * curr = (tnode *)malloc(sizeof(tnode));
    curr->op = (char *)malloc(sizeof(char));
    *(curr->op) = op;
    curr->left = l;
    curr->right = r;
    return curr;
}

/*Get postfix form*/
void postfix(tnode* node){
    if(node == NULL){
        return;
    }
    postfix(node->left);
    postfix(node->right);
    if(node->op != NULL)
        printf("%c ",*node->op);
    else
        printf("%d ",node->val);
}

/*Get prefix form */
void prefix(tnode* node){
    if(node == NULL){
        return;
    }
    if(node->op != NULL)
        printf("%c ",*node->op);
    else
        printf("%d ",node->val);
    
    prefix(node->left);
    prefix(node->right);
}

/*Allocate a free register*/
