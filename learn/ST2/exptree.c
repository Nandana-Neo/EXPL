#include "exptree.h"

tnode* makeLeafNode(int n, VarType type, char *varname){
    tnode * curr = createTree(n,type,varname,NULL,NULL);
    return curr;
}

NodeType nodeType(char* type){
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

tnode* createTree(int val, VarType type, char* c, tnode *l, tnode *r){
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
    curr->nodetype = nodeType(c);
    return curr;
}

tnode* makeOperatorNode(char c,tnode* l, tnode* r){
    tnode* curr = createTree(0,TYPE_NONE,&c,l,r);
    return curr;
}



reg_index regNum = 0;
reg_index getReg(){
    if(regNum==19){
        fprintf(stderr, "\nOut of free registers\n");  //error
        exit(1);
    }
    return regNum++;
}

int freeReg(){
    if(regNum == 0){
        fprintf(stderr,"\nNo registers to be freed\n"); //error
        exit(1);
    }
    regNum--;
    return 0;
}

// reg_index codeGen(tnode* node, FILE * fp){
//     if(node->op == NULL){
//         // leaf node
//         int i = getReg();
//         fprintf(fp,"MOV R%d, %d\n",i,node->val);
//         return i;
//     }
//     int i = codeGen(node->left, fp);
//     int j = codeGen(node->right, fp);
//     switch(*node->op){
//         case '+': 
//             fprintf(fp, "ADD R%d, R%d\n",i,j);
//             break;
//         case '-': 
//             fprintf(fp, "SUB R%d, R%d\n",i,j);
//             break;
//         case '*': 
//             fprintf(fp, "MUL R%d, R%d\n",i,j);
//             break;
//         case '/': 
//             fprintf(fp, "DIV R%d, R%d\n",i,j);
//             break;
//     }
//     freeReg();
//     return i;
// }


/*Get prefix form */
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