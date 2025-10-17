#include "code_gen.h"
#include <string.h>

static int regNum = 0;
static int label = 1;   // 0 used for end of main fn

int get_reg(){
    if(regNum==20){
        fprintf(stderr, "\nOut of free registers\n");  //error
        exit(1);
    }
    // printf("Assigned: R%d\n",regNum);  //[DEBUG]: Register
    return regNum++;
}

int free_reg(){
    if(regNum == 0){
        fprintf(stderr,"\nNo registers to be freed\n"); //error
        exit(1);
    }
    // printf("Freed: R%d\n",regNum-1);  //[DEBUG]: Register
    regNum--;
    return 0;
}


int get_label(){
    return label++;
}


loc_and_val* create_gen_node(int loc, int val){
    loc_and_val* node = (loc_and_val*)malloc(sizeof(loc_and_val));
    node->loc = loc;
    node->val = val;
    return node;
}


void free_gen_node(loc_and_val* node){
    // printf("[TRY TO FREE] Loc:R%d, Val:R%d\n",node->loc,node->val);   //[DEBUG]: Register
    if(node->loc != -1)
        free_reg();
    if(node->val != -1)
        free_reg();
    free(node);
}

loc_and_val* code_gen_ARG(tnode* node, FILE* fp, int start_label, int end_label){
    if(node == NULL)
        return NULL;
    code_gen_ARG(node->next, fp, start_label, end_label);   // last aeg to be pushed first
    loc_and_val* gen_node = code_gen(node, fp, start_label, end_label);
    fprintf(fp, "PUSH R%d\n", gen_node->val);
    free_gen_node(gen_node);
    return NULL;
}

loc_and_val* code_gen_FN_CALL(tnode* node, FILE* fp, int start_label, int end_label){
    int i = 0;
    while(i<regNum){
        fprintf(fp, "PUSH R%d\n", i);
        i++;
    }
    regNum = 0; // reset registers

    code_gen_ARG(node->left, fp, start_label, end_label);
    fprintf(fp, "PUSH R0\n");   // empty space for return register
    fprintf(fp, "CALL _F%d\n",node->gst_entry->f_label);

    regNum = i; // re-assign registers
    int return_reg = get_reg();
    int dummy_reg = (return_reg!=0)? 0 : 1;

    fprintf(fp,"POP R%d\n",return_reg);     //return value
    tnode* curr = node->left;               // POP arglist
    while(curr != NULL){
        fprintf(fp,"POP R%d\n",dummy_reg);
        curr = curr->next;
    }

    i--;
    while(i>=0){
        fprintf(fp, "POP R%d\n", i);
        i--;
    }
    return create_gen_node(-1, return_reg);
}

void push_local_decl(Lsymbol* lst, FILE* fp){
    if(lst == NULL)
        return;
    if(lst->binding > 0){    // local decl
        fprintf(fp,"PUSH R0\n");
    }
    push_local_decl(lst->next, fp);
}

void pop_local_decl(Lsymbol* lst, FILE* fp){
    if(lst == NULL)
        return;
    if(lst->binding > 0){    // local decl
        int dummy_reg = get_reg();
        fprintf(fp,"POP R%d\n",dummy_reg);
        free_reg();
    }
    pop_local_decl(lst->next, fp);
}

void code_gen_fn_begin(FILE* fp){
    // Push BP
    fprintf(fp,"PUSH BP\n");
    // Resassign BP to SP
    fprintf(fp,"MOV BP, SP\n");
    // Push space for local decl variables
    push_local_decl(curr_lsymbol, fp);
}

void code_gen_fn_end(FILE* fp){
    // Pop out local variables
    pop_local_decl(curr_lsymbol, fp);
    // Pop BP
    fprintf(fp,"POP BP\n");
    // RET instr
    fprintf(fp,"RET\n");
}

loc_and_val* code_gen_fn(tnode* node, FILE* fp){
    code_gen_fn_begin(fp);
    // code gen for fn
    code_gen(node, fp, -1, -1);
    // return statement - default
    code_gen_fn_end(fp);
    return NULL;
}

loc_and_val* code_gen_RET(tnode* node, FILE* fp, int start_label, int end_label){
    loc_and_val* e_reg = code_gen(node->left, fp, start_label, end_label);
    int addr = get_reg();
    fprintf(fp,"MOV R%d, BP\n",addr);
    fprintf(fp,"SUB R%d, 2\n",addr);
    fprintf(fp,"MOV [R%d], R%d\n", addr, e_reg->val);
    free_reg();
    free_gen_node(e_reg);
    code_gen_fn_end(fp);
    return create_gen_node(-1,-1);
}

loc_and_val* code_gen_ID(tnode* node, FILE* fp){
    Lsymbol* lst_entry = node->lst_entry;
    Gsymbol* gst_entry = node->gst_entry;
    if(lst_entry == NULL && gst_entry==NULL){
        fprintf(stderr, "Variable not declared:%s\n",node->varname);
        exit(1);
    }
    loc_and_val* ans = create_gen_node(-1,-1);
    int reg_val = get_reg();
    int reg_loc = get_reg();
    if(lst_entry != NULL){
        int location = lst_entry->binding;
        fprintf(fp,"MOV R%d, %d\n",reg_loc, location);
        fprintf(fp,"ADD R%d, BP\n",reg_loc);
    }
    else{
        int location = gst_entry->binding;
        fprintf(fp,"MOV R%d, %d\n",reg_loc,location);
    }
    fprintf(fp,"MOV R%d, [R%d]\n",reg_val,reg_loc);
    ans->loc = reg_loc;
    ans->val = reg_val;
    return ans;
}

loc_and_val* code_gen_VAL(tnode* node, FILE* fp){
    int i = get_reg();
    if(is_int(node->type_entryy))
        fprintf(fp,"MOV R%d, %d\n",i,node->val.int_val);
    else if(is_str(node->type_entryy))
        //store first 16 chars into the reg
        fprintf(fp,"MOV R%d, %s\n",i,node->val.str_val);
    
    loc_and_val* ans = create_gen_node(-1, i);
    return ans;
}

loc_and_val* code_gen_CONN(tnode* node, FILE* fp, int start_label, int end_label){
    loc_and_val* i = code_gen(node->left, fp, start_label, end_label);
    loc_and_val* j = code_gen(node->right, fp, start_label, end_label);
    free_gen_node(j);
    free_gen_node(i);
    loc_and_val* ans = create_gen_node(-1,-1);
    return ans;
}

// [num] format
// ID[i][j][k] = i*(Y*Z) + j*Z + k
loc_and_val* code_gen_INDEX(tnode* node, FILE* fp, array* size_array_ptr){
    //mulltiply index with ptr pointing number
    loc_and_val* num_gen_node = code_gen(node->left, fp, -1, -1);
    if(num_gen_node->loc != -1)
        free_reg();
    num_gen_node->loc = -1;
    
    if(node->right != NULL){
        loc_and_val* right_index_node = code_gen_INDEX(node->right, fp, size_array_ptr->nxt);
        if(right_index_node->loc != -1)
            free_reg();
        right_index_node->loc = -1;
        fprintf(fp,"MUL R%d, %d\n",right_index_node->val, size_array_ptr->val);
        fprintf(fp,"ADD R%d, R%d\n", num_gen_node->val, right_index_node->val);
        free_gen_node(right_index_node);
    }
    return num_gen_node;
       
}

// id[num] format
loc_and_val* code_gen_ARR(tnode* node, FILE* fp){
    // We only need the location of the id mainly
    if(node->left->gst_entry == NULL){
        fprintf(stderr, "Variable not declared:%s\n",node->varname);
        exit(1);
    }
    loc_and_val* id = code_gen(node->left, fp, -1, -1);        // starting loc
    if(id->loc != -1)
        free_reg();
    id->loc = -1;
    int id_loc_reg = id->val;
    loc_and_val* num_gen_node = code_gen_INDEX(node->right, fp, node->left->gst_entry->size_array);
    //[TODO] If needed, we might need to multiply the index with the size of the type stored in the array AND assign space accordingly in the beginning while decl
    int num_reg = num_gen_node->val;
    fprintf(fp,"ADD R%d, R%d\n",num_reg,id_loc_reg);    // location is in num_reg
    fprintf(fp,"MOV R%d, [R%d]\n",id_loc_reg, num_reg);
    id->loc = num_reg;
    id->val = id_loc_reg;
    if(num_gen_node->loc != -1)
        free_reg();
    free(num_gen_node);
    return id;

}

loc_and_val* code_gen_ADDR_OF(tnode* node, FILE* fp){
    // left node should be a varaible ,i.e., ID
    if(node->left->nodetype != NODE_LEAF || node->left->varname==NULL){
        fprintf(stderr, "ERROR: Reference to unknown type\n");
        exit(1);
    }
    int reg1 = get_reg();
    if(node->left->lst_entry){
        fprintf(fp, "MOV R%d, %d\n", reg1, node->left->lst_entry->binding);
        fprintf(fp, "ADD R%d, BP\n", reg1);
    }
    else{
        fprintf(fp, "MOV R%d, %d\n",reg1,node->left->gst_entry->binding);
    }
    loc_and_val* ans = create_gen_node(-1, reg1);
    return ans;
}

loc_and_val* code_gen_VAL_AT(tnode* node, FILE* fp){
    loc_and_val* ptr = code_gen(node->left, fp, -1, -1);
    if(ptr->loc != -1)
        free_reg();
    ptr->loc = get_reg();
    fprintf(fp, "MOV R%d, R%d\n",ptr->loc, ptr->val);
    fprintf(fp, "MOV R%d, [R%d]\n",ptr->val, ptr->loc);
    return ptr;
}

loc_and_val* code_gen_MEMBER_OF(tnode* node, FILE* fp){
    loc_and_val* l = code_gen(node->left, fp, -1, -1);
    char* field_name = node->right->varname;
    FieldList* field = field_list_get(field_name, node->left->type_entryy->type_table);
    // get field no from field
    fprintf(fp, "MOV R%d, R%d\n",l->loc, l->val);
    fprintf(fp, "ADD R%d, %d\n", l->loc, field->field_id);
    fprintf(fp, "MOV R%d, [R%d]\n", l->val, l->loc);
    return l;
}

loc_and_val* code_gen_initialize(tnode* node, FILE* fp){
    int reg_val = get_reg();
    for(int i=0; i<reg_val; i++){
        fprintf(fp, "PUSH R%d\n", i);
    }
    int dummy = 0;
    if(reg_val == 0)
        dummy = 1;
    fprintf(fp, "MOV R0, \"Heapset\"\n");
    fprintf(fp, "PUSH R0\n"); // Heapset
    fprintf(fp, "PUSH R0\n"); // arg1
    fprintf(fp, "PUSH R0\n"); // arg2
    fprintf(fp, "PUSH R0\n"); // arg3
    fprintf(fp, "PUSH R0\n"); // return val
    fprintf(fp, "CALL 0\n");
    fprintf(fp, "POP R%d\n",reg_val); // return val
    fprintf(fp, "POP R%d\n",dummy); // arg 3
    fprintf(fp, "POP R%d\n",dummy); // arg 2
    fprintf(fp, "POP R%d\n",dummy); // arg 1
    fprintf(fp, "POP R%d\n",dummy); // HeapSet
    loc_and_val* registers = create_gen_node(-1, reg_val);
    return registers;
}

loc_and_val* code_gen_alloc(tnode* node, FILE* fp){
    loc_and_val* sz = code_gen(node->left, fp, -1, -1);
    if(sz->loc != -1){
        free_reg();
        sz->loc = -1;
    }
    for(int i=0; i<sz->val; i++){
        fprintf(fp, "PUSH R%d\n", i);
    }
    int dummy = 0;
    if(sz->val==0)
        dummy = 1;
    fprintf(fp, "MOV R%d, \"Alloc\"\n", dummy);
    fprintf(fp, "PUSH R%d\n", dummy); // Alloc
    fprintf(fp, "PUSH R%d\n", sz->val); // arg1
    fprintf(fp, "PUSH R%d\n", dummy); // arg2
    fprintf(fp, "PUSH R%d\n", dummy); // arg3
    fprintf(fp, "PUSH R%d\n", dummy); // return val
    fprintf(fp, "CALL 0\n");
    fprintf(fp, "POP R%d\n",sz->val); // return val
    fprintf(fp, "POP R%d\n",dummy); // arg 3
    fprintf(fp, "POP R%d\n",dummy); // arg 2
    fprintf(fp, "POP R%d\n",dummy); // arg 1
    fprintf(fp, "POP R%d\n",dummy); // Alloc
    return sz;
}

loc_and_val* code_gen_free(tnode* node, FILE* fp){
    loc_and_val* addr = code_gen(node->left, fp, -1, -1);
    if(addr->loc != -1){
        free_reg();
        addr->loc = -1;
    }
    for(int i=0; i<addr->val; i++){
        fprintf(fp, "PUSH R%d\n", i);
    }
    int dummy = 0;
    if(addr->val==0)
        dummy = 1;
    fprintf(fp, "MOV R%d, \"Free\"\n", dummy);
    fprintf(fp, "PUSH R%d\n", dummy); // Free
    fprintf(fp, "PUSH R%d\n", addr->val); // arg1
    fprintf(fp, "PUSH R%d\n", dummy); // arg2
    fprintf(fp, "PUSH R%d\n", dummy); // arg3
    fprintf(fp, "PUSH R%d\n", dummy); // return val
    fprintf(fp, "CALL 0\n");
    fprintf(fp, "POP R%d\n",addr->val); // return val
    fprintf(fp, "POP R%d\n",dummy); // arg 3
    fprintf(fp, "POP R%d\n",dummy); // arg 2
    fprintf(fp, "POP R%d\n",dummy); // arg 1
    fprintf(fp, "POP R%d\n",dummy); // Alloc
    return addr;
}

loc_and_val* code_gen_OP(tnode* node, FILE* fp){
    loc_and_val* i = code_gen(node->left, fp, -1, -1);
    loc_and_val* j = code_gen(node->right, fp, -1, -1);
    switch(node->nodetype){
        case NODE_ADD: 
            fprintf(fp, "ADD R%d, R%d\n",i->val,j->val);
            break;
        case NODE_SUB: 
            fprintf(fp, "SUB R%d, R%d\n",i->val,j->val);
            break;
        case NODE_MUL: 
            fprintf(fp, "MUL R%d, R%d\n",i->val,j->val);
            break;
        case NODE_DIV: 
            fprintf(fp, "DIV R%d, R%d\n",i->val,j->val);
            break;
        case NODE_MOD: 
            fprintf(fp, "MOD R%d, R%d\n",i->val,j->val);
            break;
        case NODE_GT:
            fprintf(fp, "GT R%d, R%d\n",i->val,j->val);
            break;
        case NODE_LT:
            fprintf(fp, "LT R%d, R%d\n",i->val,j->val);
            break;
        case NODE_GE:
            fprintf(fp, "GE R%d, R%d\n",i->val,j->val);
            break;
        case NODE_LE:
            fprintf(fp, "LE R%d, R%d\n",i->val,j->val);
            break;
        case NODE_EQ:
            fprintf(fp, "EQ R%d, R%d\n",i->val,j->val);
            break;
        case NODE_NE:
            fprintf(fp, "NE R%d, R%d\n",i->val,j->val);
            break;
        case NODE_AND:
            fprintf(fp, "ADD R%d, R%d\n",i->val,j->val);
            fprintf(fp, "MOV R%d, 2\n", j->val);
            fprintf(fp, "EQ R%d, R%d\n", i->val, j->val);
            break;
        case NODE_OR:
            fprintf(fp, "ADD R%d, R%d\n",i->val,j->val);
            fprintf(fp, "MOV R%d, 1\n", j->val);
            fprintf(fp, "GE R%d, R%d\n", i->val, j->val);
            break;
        case NODE_NOT:
            // right is NULL, so j is NULL
            fprintf(fp, "ADD R%d, 1\n", i->val);
            fprintf(fp, "MOD R%d, 2\n", i->val);
            break;
        case NODE_ASGN:
            fprintf(fp, "MOV [R%d], R%d\n",i->loc,j->val);
            break;
    }
    free_gen_node(j);
    if(node->nodetype == NODE_ASGN){
        if(i->loc != -1)
            free_reg();
        if(i->val != -1)
            free_reg();
        i->val = -1;
        i->loc = -1;
    }
    return i;
}

loc_and_val* code_gen_READ(tnode* node, FILE* fp){
    // only left node will be there and that will be variable name
    tnode* var_node = node->left;
    Lsymbol* lst_entry = node->left->lst_entry;
    Gsymbol* gst_entry = node->left->gst_entry;
    if(lst_entry == NULL && gst_entry == NULL && !(node->left->nodetype==NODE_ARR || node->left->nodetype==NODE_VAL_AT)){
        fprintf(stderr, "READ: Variable not declared:%s\n",var_node->varname);
        exit(1);
    }
    int location;
    loc_and_val* l_gen_node = NULL;
    if(node->left->nodetype == NODE_ARR || node->left->nodetype==NODE_VAL_AT){
        l_gen_node = code_gen(var_node, fp , -1, -1);
        location = l_gen_node->loc; // register number storing the location
    }
    else{
        location = get_reg();
        if(lst_entry!=NULL){
            fprintf(fp,"MOV R%d, %d\n",location, lst_entry->binding);
            fprintf(fp,"ADD R%d, BP\n", location);
        }
        else{
            fprintf(fp,"MOV R%d, %d\n", location, gst_entry->binding);
        }
        l_gen_node = create_gen_node(-1, location);
    }
    for(int i=0;i < l_gen_node->val; i++){ // the val has the register number that stores the return value
        fprintf(fp,"PUSH R%d\n",i);
    }
    int dupl = 0;
    if(location <= 1){
        dupl = 2;
    }
    // Use R0 after pushing all registers
    // Call get_reg() to use the variable to store return value
    fprintf(fp,"MOV R%d, \"Read\"\n", dupl);
    fprintf(fp,"PUSH R%d\n", dupl);
    fprintf(fp,"MOV R%d, -1\n", dupl);
    fprintf(fp,"PUSH R%d\n",dupl);
    // location to which data read is to be stored
    fprintf(fp,"PUSH R%d\n",location);  
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"PUSH R0\n");
    fprintf(fp,"CALL 0\n");
    if(l_gen_node->loc != -1){ // array
        // free location reg, use val reg for storage of return value
        free_reg();
        l_gen_node->loc = -1;
        location = l_gen_node->val;
    }
    fprintf(fp,"POP R%d\n",location);    //return value
    fprintf(fp,"POP R%d\n",dupl);
    fprintf(fp,"POP R%d\n",dupl);
    fprintf(fp,"POP R%d\n",dupl);
    fprintf(fp,"POP R%d\n",dupl);

    for(int i=location-1;i>=0;i--){
        fprintf(fp,"POP R%d\n",i);
    }
    return l_gen_node;
}


loc_and_val* code_gen_WRITE(tnode* node, FILE* fp){
    // only left node will be there and that will have the value stored in reg
    loc_and_val* l_gen_node = code_gen(node->left,fp, -1, -1);
    int reg = l_gen_node->val;
    if(l_gen_node->loc != -1)
        free_reg();
    l_gen_node->loc = -1;
    int dupl = 0;
    if(reg == 0)
        dupl = 1;
    for(int i=0;i<reg;i++){
        fprintf(fp,"PUSH R%d\n",i);
    }
    // Use R0 after pushing all registers
    // Use the already in use register to give input data and store return value
    fprintf(fp,"MOV R%d, \"Write\"\n",dupl);
    fprintf(fp,"PUSH R%d\n",dupl);
    fprintf(fp,"MOV R%d, -2\n",dupl);
    fprintf(fp,"PUSH R%d\n",dupl);
    fprintf(fp,"PUSH R%d\n",reg);
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
    return l_gen_node;
}

loc_and_val* code_gen_IF(tnode* node, FILE* fp, int start_label, int end_label){
    loc_and_val* l_gen_node = code_gen(node->left, fp, start_label, end_label);    // code_gen(E)
    if(l_gen_node->loc != -1)
        free_reg();
    int l1 = get_label();
    fprintf(fp,"JZ R%d, _L%d\n",l_gen_node->val,l1);
    loc_and_val* m_gen_node = code_gen(node->middle, fp, start_label, end_label); // code_gen(S1)

    free_gen_node(m_gen_node);

    int l2 = -1;
    if(node->right){    // if else node
        l2 = get_label();
        fprintf(fp,"JMP _L%d\n",l2);
    }
    fprintf(fp,"_L%d:\n",l1);
    if(node->right){    // if else node
        loc_and_val* r_gen_node = code_gen(node->right, fp, start_label, end_label);  // code_gen(S2)
        free_gen_node(r_gen_node);
        fprintf(fp,"_L%d:\n",l2);
    }     
    free_reg(); // for l->val register
    l_gen_node->val = -1;
    l_gen_node->loc = -1;
    return l_gen_node;
}

loc_and_val* code_gen_DO_WHILE(tnode* node, FILE* fp){
    int l1 = get_label();   // start label
    int l2 = get_label();   // end label
    fprintf(fp,"_L%d:\n",l1);

    loc_and_val* l = code_gen(node->left,fp, l1, l2);   // code_gen(Slist)
    free_gen_node(l);
    loc_and_val* r = code_gen(node->right, fp, l1, l2);  // code_gen(E)
    fprintf(fp,"JNZ R%d, _L%d\n",r->val,l1);
    fprintf(fp, "_L%d:\n",l2);
    free_gen_node(r);

    loc_and_val* ans = create_gen_node(-1, -1);
    return ans;
}

loc_and_val* code_gen_REPEAT(tnode* node, FILE* fp){
    int l1 = get_label();   // start label
    int l2 = get_label();   // end label
    fprintf(fp,"_L%d:\n",l1);

    loc_and_val* l = code_gen(node->left,fp, l1, l2);   // code_gen(Slist)
    free_gen_node(l);
    loc_and_val* r = code_gen(node->right, fp, l1, l2);  // code_gen(E)
    fprintf(fp,"JZ R%d, _L%d\n",r->val,l1);
    fprintf(fp, "_L%d:\n",l2);
    free_gen_node(r);

    loc_and_val* ans = create_gen_node(-1, -1);
    return ans;
}

loc_and_val* code_gen_WHILE(tnode* node, FILE* fp){
    int l1 = get_label();   // start label
    int l2 = get_label();   // end label
    fprintf(fp,"_L%d:\n",l1);

    loc_and_val* l = code_gen(node->left,fp, l1, l2);   // code_gen(E)
    fprintf(fp,"JZ R%d, _L%d\n",l->val,l2);
    loc_and_val* r = code_gen(node->right, fp, l1, l2);  // code_gen(S1)
    fprintf(fp,"JMP _L%d\n",l1);
    fprintf(fp, "_L%d:\n",l2);

    free_gen_node(r);
    free_gen_node(l);

    loc_and_val* ans = create_gen_node(-1, -1);
    return ans;
}

loc_and_val* code_gen_BREAK(tnode* node, FILE* fp, int end_label){
    if(end_label != -1)
        fprintf(fp, "JMP _L%d\n",end_label);
    loc_and_val* ans = create_gen_node(-1, -1);
    return ans;
}

loc_and_val* code_gen_CONTINUE(tnode* node, FILE* fp, int start_label){
    if(start_label!=-1)
        fprintf(fp, "JMP _L%d\n",start_label);
    loc_and_val* ans = create_gen_node(-1, -1);
    return ans;
}


loc_and_val* code_gen(tnode* node, FILE* fp, int start_label, int end_label){
    if(node == NULL){
        return NULL;
    }
    switch(node->nodetype){
        case NODE_LEAF:
            if(node->varname == NULL)  // NUM
                return code_gen_VAL(node, fp);
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
        case NODE_ARR:
            return code_gen_ARR(node, fp);
        case NODE_ADDR_OF:
            return code_gen_ADDR_OF(node, fp);
        case NODE_VAL_AT:
            return code_gen_VAL_AT(node, fp);
        case NODE_FN:
            return code_gen_FN_CALL(node, fp, start_label, end_label);
        case NODE_RET:
            return code_gen_RET(node, fp, start_label, end_label);
        case NODE_MEMBER_OF:
            return code_gen_MEMBER_OF(node, fp);
        case NODE_INITIALIZE:
            return code_gen_initialize(node, fp);
        case NODE_ALLOC:
            return code_gen_alloc(node, fp);
        case NODE_FREE:
            return code_gen_free(node, fp);
        default:
            return code_gen_OP(node, fp);
    }
}

void code_gen_start(FILE* fp){
    fprintf(fp,"0\n2056\n0\n0\n0\n0\n0\n0\n");
}

void code_gen_SP_init(FILE* fp){
    fprintf(fp,"MOV SP, %d\n",SP-1);
}

void code_gen_final(FILE* fp){
    fprintf(fp,"_L0:\n");
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
                if(is_int(node->type_entryy))
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