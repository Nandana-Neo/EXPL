#include "code_gen.h"
#include <string.h>

static int regNum = 0;
static int registerTable[20];   // used for assignment operator with ID = E
static int label = 0;

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


int get_label(){
    return label++;
}

int code_gen_ID(tnode* node, FILE* fp){
    Gsymbol * symbol_table_entry = get_variable(node->varname);
    if(symbol_table_entry == NULL){
        fprintf(stderr, "Variable not declared:%s\n",node->varname);
        exit(1);
    }
    int location = symbol_table_entry->binding;
    int reg = get_reg();
    registerTable[reg] = location;
    fprintf(fp,"MOV R%d, [%d]\n",reg,location);
    return reg;
}

int code_gen_NUM(tnode* node, FILE* fp){
    int i = get_reg();
    fprintf(fp,"MOV R%d, %d\n",i,node->val.int_val);
    return i;
}

int code_gen_CONN(tnode* node, FILE* fp, int start_label, int end_label){
    int i = code_gen(node->left, fp, start_label, end_label);
    int j = code_gen(node->right, fp, start_label, end_label);
    if(i!=-1)
        free_reg(); //extra free reg to ensure that both the left and right statements' assigned regs are freed
    if(j!=-1)
        free_reg();
    i = -1;
    j = -1;
    return i;
}

int code_gen_OP(tnode* node, FILE* fp){
    int i = code_gen(node->left, fp, -1, -1);
    int j = code_gen(node->right, fp, -1, -1);
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
        case NODE_GT:
            fprintf(fp, "GT R%d, R%d\n",i,j);
            break;
        case NODE_LT:
            fprintf(fp, "LT R%d, R%d\n",i,j);
            break;
        case NODE_GE:
            fprintf(fp, "GE R%d, R%d\n",i,j);
            break;
        case NODE_LE:
            fprintf(fp, "LE R%d, R%d\n",i,j);
            break;
        case NODE_EQ:
            fprintf(fp, "EQ R%d, R%d\n",i,j);
            break;
        case NODE_NE:
            fprintf(fp, "NE R%d, R%d\n",i,j);
            break;
        case NODE_ASGN:
            fprintf(fp, "MOV [%d], R%d\n",registerTable[i],j);
            free_reg();  //extra free reg to free the LHS as well
            registerTable[i] = -1;
            i = -1;
            break;
    }
    free_reg();
    return i;
}

int code_gen_READ(tnode* node, FILE* fp){
    // only left node will be there and that will be variable name
    tnode* var_node = node->left;
    Gsymbol * symbol_table_entry = get_variable(var_node->varname);
    if(symbol_table_entry == NULL){
        fprintf(stderr, "Variable not declared:%s\n",var_node->varname);
        exit(1);
    }
    int location = symbol_table_entry->binding;
    int reg = get_reg();
    for(int i=0;i<reg;i++){
        fprintf(fp,"PUSH R%d\n",i);
    }
    int dupl = 0;
    if(reg == 0){
        dupl = 1;
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
    fprintf(fp,"POP R%d\n",dupl);
    fprintf(fp,"POP R%d\n",dupl);
    fprintf(fp,"POP R%d\n",dupl);
    fprintf(fp,"POP R%d\n",dupl);

    for(int i=reg-1;i>=0;i--){
        fprintf(fp,"POP R%d\n",i);
    }
    return reg;
}


int code_gen_WRITE(tnode* node, FILE* fp){
    // only left node will be there and that will have the value stored in reg
    int reg = code_gen(node->left,fp, -1, -1);
    int j = get_reg();
    fprintf(fp,"MOV R%d, R%d\n", j,reg);

    for(int i=0;i<reg;i++){
        fprintf(fp,"PUSH R%d\n",i);
    }
    // Use R0 after pushing all registers
    // Use the already in use register to give input data and store return value
    fprintf(fp,"MOV R0, \"Write\"\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"MOV R0, -2\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R%d\n",j);
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"CALL 0\n");
    fprintf(fp,"POP R%d\n",j);    //return value
    fprintf(fp,"POP R0\n");
    fprintf(fp,"POP R0\n");
    fprintf(fp,"POP R0\n");
    fprintf(fp,"POP R0\n");
    fprintf(fp,"MOV R%d, R%d\n", reg, j);
    free_reg();
    
    for(int i=reg-1;i>=0;i--){
        fprintf(fp,"POP R%d\n",i);
    }
    return reg;
}

int code_gen_IF(tnode* node, FILE* fp, int start_label, int end_label){
    int i = code_gen(node->left,fp, start_label, end_label);    // code_gen(E)
    int l1 = get_label();
    fprintf(fp,"JZ R%d, _L%d\n",i,l1);
    code_gen(node->middle, fp, start_label, end_label); // code_gen(S1)
    int l2 = -1;
    if(node->right){    // if else node
        l2 = get_label();
        fprintf(fp,"JMP _L%d\n",l2);
    }
    fprintf(fp,"_L%d:\n",l1);
    if(node->right){    // if else node
        code_gen(node->right, fp, start_label, end_label);  // code_gen(S2)
        fprintf(fp,"_L%d:\n",l2);
    }     
    free_reg(); // for the i generated by E
    return -1;
}

int code_gen_DO_WHILE(tnode* node, FILE* fp){
    int l1 = get_label();   // start label
    int l2 = get_label();   // end label
    fprintf(fp,"_L%d:\n",l1);
    code_gen(node->left,fp, l1, l2);   // code_gen(Slist)
    int i = code_gen(node->right, fp, l1, l2);  // code_gen(E)
    fprintf(fp,"JNZ R%d, _L%d\n",i,l1);
    fprintf(fp, "_L%d:\n",l2);
    free_reg(); // For the reg i used by E
    return -1;
}

int code_gen_REPEAT(tnode* node, FILE* fp){
    int l1 = get_label();   // start label
    int l2 = get_label();   // end label
    fprintf(fp,"_L%d:\n",l1);
    code_gen(node->left,fp, l1, l2);   // code_gen(Slist)
    int i = code_gen(node->right, fp, l1, l2);  // code_gen(E)
    fprintf(fp,"JZ R%d, _L%d\n",i,l1);
    fprintf(fp, "_L%d:\n",l2);
    free_reg(); // For the reg i used by E
    return -1;
}

int code_gen_WHILE(tnode* node, FILE* fp){
    int l1 = get_label();   // start label
    int l2 = get_label();   // end label
    fprintf(fp,"_L%d:\n",l1);
    int i = code_gen(node->left,fp, l1, l2);   // code_gen(E)
    fprintf(fp,"JZ R%d, _L%d\n",i,l2);
    code_gen(node->right, fp, l1, l2);  // code_gen(S1)
    fprintf(fp,"JMP _L%d\n",l1);
    fprintf(fp, "_L%d:\n",l2);
    free_reg(); // For the reg i used by E
    return -1;
}

int code_gen_BREAK(tnode* node, FILE* fp, int end_label){
    if(end_label != -1)
        fprintf(fp, "JMP _L%d\n",end_label);
    return -1;
}

int code_gen_CONTINUE(tnode* node, FILE* fp, int start_label){
    if(start_label!=-1)
        fprintf(fp, "JMP _L%d\n",start_label);
    return -1;
}


int code_gen(tnode* node, FILE* fp, int start_label, int end_label){
    switch(node->nodetype){
        case NODE_LEAF:
            if(node->varname == NULL)  // NUM
                return code_gen_NUM(node, fp);
            else    //ID
                return code_gen_ID(node, fp);

        case NODE_READ:
            return code_gen_READ(node, fp);

        case NODE_WRITE:
            return code_gen_WRITE(node, fp);

        case NODE_IF:
        case NODE_IFELSE:
            return code_gen_IF(node, fp, start_label, end_label);
        
        case NODE_WHILE:
            return code_gen_WHILE(node, fp);
        
        case NODE_REPEAT:
            return code_gen_REPEAT(node,fp);

        case NODE_DOWHILE:
            return code_gen_DO_WHILE(node,fp);

        case NODE_BREAK:
            return code_gen_BREAK(node, fp, end_label);

        case NODE_CONTINUE:
            return code_gen_CONTINUE(node, fp, start_label);

        case NODE_CONN:
            return code_gen_CONN(node, fp, start_label, end_label);
        default:
            return code_gen_OP(node, fp);
    }
    return -1;
}

void code_gen_start(FILE * fp){
    fprintf(fp,"0\n2056\n0\n0\n0\n0\n0\n0\n");
    fprintf(fp,"MOV SP, %d\n",SP);
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


// Only for the evaluator excercise
int storage[26];


int evaluate(tnode* node){
    int i,j;
    switch(node->nodetype){
        case NODE_LEAF:
            if(node->varname == NULL){  // NUM/STR/CHAR
                if(node->type == TYPE_INT)
                    return node->val.int_val;   // NUM
                else 
                    return 0;
            }
            else    //ID
                return storage[(*node->varname)-'a'];

        case NODE_READ:
            i = *(node->left->varname) - 'a';
            scanf("%d",&storage[i]);
            return storage[i];

        case NODE_WRITE:
            int val = evaluate(node->left);
            printf("%d\n",val);
            return val;
        
        case NODE_CONN:
            evaluate(node->left);
            evaluate(node->right);
            return -1;
        
        case NODE_ASGN:
            i = *(node->left->varname) - 'a';   // left will be ID
            j = evaluate(node->right);
            storage[i] = j;
            return j;

        case NODE_IF:
            i = evaluate(node->left);
            if(i==1)
                evaluate(node->middle);
            return -1;
        
        case NODE_IFELSE:
            i = evaluate(node->left);
            if(i==1)
                evaluate(node->middle);
            else
                evaluate(node->right);
            return -1;
        
        case NODE_WHILE:
            i = evaluate(node->left);
            while(i==1){
                evaluate(node->right);
                i = evaluate(node->left);
            }
            return -1;

        default:
            i = evaluate(node->left);
            j = evaluate(node->right);
            switch(node->nodetype){
                case NODE_ADD:
                    return i+j;
                case NODE_SUB:
                    return i-j;
                case NODE_MUL:
                    return i*j;
                case NODE_DIV:
                    return i/j;
                case NODE_GT:
                    return i>j;
                case NODE_LT:
                    return i<j;
                case NODE_GE:
                    return i>=j;
                case NODE_LE:
                    return i<=j;
                case NODE_EQ:
                    return i==j;
                case NODE_NE:
                    return i!=j;
            }
    }
    return -1;
}