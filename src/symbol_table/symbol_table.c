#include "symbol_table.h"

int SP = 4096;
static int f_label = 0;

int get_f_label(){
    return f_label++;
}

Gsymbol* add_variable(char* name, int size, VarType type){
    if(get_variable(name)!=NULL){
        fprintf(stderr,"Variable redeclared:%s\n",name);
        exit(1);
    }
    Gsymbol* node = (Gsymbol*)malloc(sizeof(Gsymbol));
    node->name = strdup(name);
    node->type = type;
    node->size = size;
    node->binding = SP;
    SP+=size;
    node->symbol_type = SYMBOL_VAR;
    node->next = symbol_table;
    node->size_array = NULL;
    node->f_label = -1;
    node->param_list = NULL;
    symbol_table = node;
    return node;
}

Gsymbol* add_fn(char* name, VarType return_type, parameter* param_list){
    if(get_variable(name)!=NULL){
        fprintf(stderr,"Variable redeclared:%s\n",name);
        exit(1);
    }
    Gsymbol* node = (Gsymbol*)malloc(sizeof(Gsymbol));
    node->name = strdup(name);
    node->symbol_type = SYMBOL_FN;
    node->param_list = param_list;
    node->type = return_type;
    node->f_label = get_f_label();
    node->binding = -1;
    node->size_array = NULL;
    node->size = 0;
    node->next = symbol_table;
    symbol_table = node;
    return node;
}

Gsymbol* get_variable(char* name){
    Gsymbol* node = symbol_table;
    while(node!=NULL){
        if(strcmp(node->name,name)==0){
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void add_array_to_symbol(FILE* fp, char* varname,char* lengths, VarType type, int sz){
    type = pointer_type(type);
    sz+=1;  // first mem stores the arr variable which points to the nxt location
    char* token = strtok(lengths, ",");
    int elem;
    array* head = NULL;
    while(token != NULL){
        elem = atoi(token);
        head = add_array_node(head, elem);
        token = strtok(NULL,",");
    }
    Gsymbol* node = add_variable(varname, sz, type);
    fprintf(fp,"MOV [%d],%d\n",node->binding,node->binding+1);
    node->size_array = head;
    node->symbol_type = SYMBOL_ARR;
    free(lengths);
    free(varname);
}

void create_entries(decl_node * ls, VarType type, FILE* fp){
    decl_node * curr = ls;
    while(curr != NULL){
        char* varname = strdup(curr->varname);
        char* lengths = strdup(curr->varname);
        int dimension = -1;
        sscanf(curr->varname,"%s %d %s",varname,&dimension,lengths);  //for arrays: "ID 2 10,10" for the case of ID[10][10]
        // printf("[DEBUG]VAR: %s", lengths);
        if(dimension>=1){    //array
            add_array_to_symbol(fp, varname, lengths, type, curr->size);
        }
        else{
            if(dimension == 0) // pointer
                type = pointer_type(type);
            // normal node
            Gsymbol* node = add_variable(varname, curr->size, type);
            free(varname);
            node->size_array = NULL;
        }
        decl_node* prev = curr;
        curr = curr->next;
        free_decl_node(prev);
    }
}

void print_st(){
    Gsymbol * curr = symbol_table;
    while(curr != NULL){
        printf("%s-%d-%d\n",curr->name,curr->size,curr->binding);
        print_array_int(curr->size_array);
        curr = curr->next;
    }
}

void print_array_int(array * arr){
    array* node = arr;
    while(node!=NULL){
        printf("%d->",node->val);
        node = node->nxt;
    }
    printf("\n");
}

array* add_array_node(array* arr,int val){
    array* node = (array *)malloc(sizeof(array));
    node->val = val;
    //insert at the beginning
    node->nxt = arr;
    return node;
}