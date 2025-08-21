#include "code_gen.h"

static int regNum = 0;
static int symbolTable[20];

int get_reg(){
    if(regNum==19){
        fprintf(stderr, "\nOut of free registers\n");  //error
        exit(1);
    }
    return regNum++;
}

int free_reg(){
    if(regNum == 0){
        fprintf(stderr,"\nNo registers to be freed\n"); //error
        exit(1);
    }
    regNum--;
    return 0;
}

int code_gen_ID(tnode* node, FILE* fp){
    //** Assuming varname is [a-z] only
    char varname = *(node->varname);
    int location = varname - 'a' + SP;
    int reg = get_reg();
    symbolTable[reg] = location;
    fprintf(fp,"MOV R%d, [%d]\n",reg,location);
    return reg;
}

int code_gen_NUM(tnode* node, FILE* fp){
    int i = get_reg();
    fprintf(fp,"MOV R%d, %d\n",i,node->val);
    return i;
}

int code_gen_OP(tnode* node, FILE* fp){
    int i = code_gen(node->left, fp);
    int j = code_gen(node->right, fp);
    switch(node->nodetype){
        case NODE_ADD: 
            fprintf(fp, "ADD R%d, R%d\n",i,j);
            break;
        case NODE_SUB: 
            fprintf(fp, "SUB R%d, R%d\n",i,j);
            break;
        case NODE_MUL: 
            fprintf(fp, "MUL R%d, R%d\n",i,j);
            break;
        case NODE_DIV: 
            fprintf(fp, "DIV R%d, R%d\n",i,j);
            break;
        case NODE_EQ:
            fprintf(fp, "MOV [%d], R%d\n",symbolTable[i],j);
            free_reg();  //extra free reg to free the LHS as well
            symbolTable[i] = -1;
            break;
        case NODE_CONN:
            if(i!=-1 && j!=-1)
                free_reg(); //extra free reg to ensure that both the left and right statements' assigned regs are freed
            return -1;
    }
    free_reg();
    return i;
}

int code_gen_READ(tnode* node, FILE *fp){
    // only left node will be there and that will be variable name
    tnode* var_node = node->left;
    char varname = *(var_node->varname);
    int location = varname - 'a' + SP;
    int reg = get_reg();
    for(int i=0;i<reg;i++){
        fprintf(fp,"PUSH R%d\n",i);
    }
    // Use R0 after pushing all registers
    // Call get_reg() to use the variable to store return value
    fprintf(fp,"MOV R0, \"Read\"\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"MOV R0, -1\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"MOV R0, %d\n",location);    // location to which data read is to be stored
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"CALL 0\n");
    fprintf(fp,"POP R%d\n",reg);    //return value
    fprintf(fp,"POP R0\n");
    fprintf(fp,"POP R0\n");
    fprintf(fp,"POP R0\n");
    fprintf(fp,"POP R0\n");

    for(int i=reg-1;i>=0;i--){
        fprintf(fp,"POP R%d\n",i);
    }
    return reg;
}


int code_gen_WRITE(tnode* node, FILE *fp){
    // only left node will be there and that will have the value stored in reg
    int reg = code_gen(node->left,fp);

    for(int i=0;i<reg;i++){
        fprintf(fp,"PUSH R%d\n",i);
    }
    // Use R0 after pushing all registers
    // Use the already in use register to give input data and store return value
    fprintf(fp,"MOV R0, \"Write\"\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"MOV R0, -2\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R%d\n",reg);
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"CALL 0\n");
    fprintf(fp,"POP R%d\n",reg);    //return value
    fprintf(fp,"POP R0\n");
    fprintf(fp,"POP R0\n");
    fprintf(fp,"POP R0\n");
    fprintf(fp,"POP R0\n");

    for(int i=reg-1;i>=0;i--){
        fprintf(fp,"POP R%d\n",i);
    }
    return reg;
}

int code_gen(tnode* node, FILE * fp){
    switch(node->nodetype){
        case NODE_LEAF:
            if(node->varname == NULL)  // NUM
                return code_gen_NUM(node, fp);
            else    //ID
                return code_gen_ID(node, fp);

        case NODE_READ:
            return code_gen_READ(node, fp);

        case NODE_WRITE:
            return code_gen_WRITE(node,fp);

        default:
            return code_gen_OP(node, fp);
    }
    return -1;
}

void code_gen_start(FILE * fp){
    fprintf(fp,"0\n2056\n0\n0\n0\n0\n0\n0\n");
    fprintf(fp,"MOV SP, 4122\n");
}

void code_gen_final(FILE * fp){
    fprintf(fp,"MOV R0,\"Exit\"\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"CALL 0");
}