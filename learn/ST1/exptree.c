#include "exptree.h"

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
reg_index regNum = 0;
reg_index getReg(){
    if(regNum==19){
        fprintf(stderr, "\nOut of free registers\n");  //error
        exit(1);
    }
    return regNum++;
}

/*Free an allocated register*/
int freeReg(){
    if(regNum == 0){
        fprintf(stderr,"\nNo registers to be freed\n"); //error
        exit(1);
    }
    regNum--;
    return 0;
}

/*Generate assembly code corresponding to AST node*/
/*Note: It will print newline at the end*/
reg_index codeGen(tnode* node, FILE * fp){
    if(node->op == NULL){
        // leaf node
        int i = getReg();
        fprintf(fp,"MOV R%d, %d\n",i,node->val);
        return i;
    }
    int i = codeGen(node->left, fp);
    int j = codeGen(node->right, fp);
    switch(*node->op){
        case '+': 
            fprintf(fp, "ADD R%d, R%d\n",i,j);
            break;
        case '-': 
            fprintf(fp, "SUB R%d, R%d\n",i,j);
            break;
        case '*': 
            fprintf(fp, "MUL R%d, R%d\n",i,j);
            break;
        case '/': 
            fprintf(fp, "DIV R%d, R%d\n",i,j);
            break;
    }
    freeReg();
    return i;
}
