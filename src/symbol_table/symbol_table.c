#include "symbol_table.h"

int SP = 4096;
static int f_label = 0;

int get_f_label(){
    return f_label++;
}

Gsymbol* add_variable_to_symbol(char* name, int size, VarType type){
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

Gsymbol* add_fn_to_symbol(char* name, VarType return_type, parameter* param_list){
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

Gsymbol* add_array_to_symbol(FILE* fp, char* varname,array* array_sz, VarType type, int sz){
    type = pointer_type(type);
    sz+=1;  // first mem stores the arr variable which points to the nxt location
    Gsymbol* node = add_variable_to_symbol(varname, sz, type);
    fprintf(fp,"MOV [%d],%d\n",node->binding,node->binding+1);
    node->size_array = array_sz;
    node->symbol_type = SYMBOL_ARR;
    return node;
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

parameter* create_parameter(char* name, VarType type){
    parameter* node = (parameter *)malloc(sizeof(parameter));
    node->name = name;
    node->type = type;
    node->next = NULL;
    return node;
}

parameter* add_parameter_to_list(parameter* lst, parameter* curr){
    //add to head of list
    curr->next = lst;
    return curr;
}


int same_parameter_list(parameter* l1, parameter* l2){
    parameter* n1 = l1;
    parameter* n2 = l2;
    while(n1!=NULL && n2!=NULL){
        if(n1->type != n2->type)
            return 0;
        if(strcmp(n1->name,n2->name)!=0)
            return 0;
        n1 = n1->next;
        n2 = n2->next;
    }
    if(n1!=NULL || n2!=NULL)
        return 0;
    return 1;
}

void free_param_list(parameter* ls){
    if(ls == NULL)
        return;
    free(ls->next);
    free(ls->name);
}

/***********************Local Symbol Table Fns*********************/ 
Lsymbol* create_lsymbol(char* varname, VarType type, int binding, Lsymbol* next){
    Lsymbol* node = (Lsymbol*)malloc(sizeof(Lsymbol));
    node->varname = strdup(varname);
    node->type = type;
    node->binding = 0;
    node->next = next;
    return node;
}

Lsymbol* connect_lsymbol(Lsymbol* lst1, Lsymbol* lst2){
    if(lst1 == NULL)
        return lst2;
    if(lst2 == NULL)
        return lst1;
    Lsymbol* curr = lst1;
    while(curr->next != NULL){
        curr = curr->next;
    }
    curr->next = lst2;
    return lst1;
}

Lsymbol* update_type_lsymbol_tb(Lsymbol* lst, VarType type){
    Lsymbol* curr = lst;
    while(curr!=NULL){
        switch (curr->type){
        case TYPE_INT:
            curr->type = type;
            break;
        
        case TYPE_INT_PTR:
            curr->type = pointer_type(type);
            break;      
        }
        curr = curr->next;
    }
    return lst;
}

Lsymbol* get_lsymbol(Lsymbol* ls, char* name){
    Lsymbol* node = ls;
    while(node!=NULL){
        if(strcmp(node->varname,name)==0){
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void free_lsymbol(Lsymbol* ls){
    if(ls==NULL)
        return;
    free_lsymbol(ls->next);
    free(ls->varname);
    free(ls);
}


Lsymbol* add_paramlist_lsymbol(parameter* param_ls, Lsymbol* tb){
    if(param_ls == NULL)
        return tb;
    Lsymbol* curr = add_paramlist_lsymbol(param_ls->next, tb);
    int logical_addr = -3;
    if(curr != tb){ // not first argument
        logical_addr = curr->binding-1;
    }
    curr = create_lsymbol(param_ls->name, param_ls->type, logical_addr, curr);
    return curr;
}