#include "symbol_table.h"

int SP = 4096;
static int f_label = 2;
Gsymbol * symbol_table = NULL;

int get_f_label(){
    return f_label++;
}

Gsymbol* add_variable_to_symbol(char* name, int size, Type* type, FILE* fp){
    if(get_variable_gst(name)!=NULL){
        fprintf(stderr,"Variable redeclared:%s\n",name);
        exit(1);
    }
    Gsymbol* node = (Gsymbol*)malloc(sizeof(Gsymbol));
    node->name = strdup(name);
    // duplicate the type
    node->type_entryy = create_type(type->type_table, type->ptr);
    node->binding = SP;
    node->cbinding = SP;
    SP+=size;
    if(type->c_type){
        fprintf(fp, "MOV [%d], %d\n",SP,type->c_type->class_id);
        node->cbinding = SP;
        size++;
        SP++;
    }
    node->size = size;
    node->symbol_type = SYMBOL_VAR;
    node->next = symbol_table;
    node->size_array = NULL;
    node->f_label = -1;
    node->param_list = NULL;
    symbol_table = node;
    return node;
}

Gsymbol* add_fn_to_symbol(char* name, Type* return_type, parameter* param_list){
    if(get_variable_gst(name)!=NULL){
        fprintf(stderr,"Variable redeclared:%s\n",name);
        exit(1);
    }
    Gsymbol* node = (Gsymbol*)malloc(sizeof(Gsymbol));
    node->name = strdup(name);
    node->symbol_type = SYMBOL_FN;
    node->param_list = param_list;
    node->type_entryy = create_type(return_type->type_table, return_type->ptr);
    node->f_label = get_f_label();
    node->binding = -1;
    node->size_array = NULL;
    node->size = 0;
    node->next = symbol_table;
    symbol_table = node;
    return node;
}

Gsymbol* get_variable_gst(char* name){
    Gsymbol* node = symbol_table;
    while(node!=NULL){
        if(strcmp(node->name,name)==0){
            return node;
        }
        node = node->next;
    }
    return NULL;
}

Lsymbol* get_variable_lst(char* name, Lsymbol* lst){
    Lsymbol* node = lst;
    while(node!=NULL){
        if(strcmp(node->varname,name)==0){
            return node;
        }
        node = node->next;
    }
    return NULL;
}

Gsymbol* add_array_to_symbol(FILE* fp, char* varname,array* array_sz, Type* type, int sz){
    type->ptr = 1;
    sz+=1;  // first mem stores the arr variable which points to the nxt location
    Gsymbol* node = add_variable_to_symbol(varname, sz, type, fp);
    fprintf(fp,"MOV [%d],%d\n",node->binding,node->binding+1);
    node->size_array = array_sz;
    node->symbol_type = SYMBOL_ARR;
    return node;
}

void print_st(){
    Gsymbol * curr = symbol_table;
    printf("\n\n");
    printf("|*************************************************************************************************************************|\n");
    printf("|*********************************************  Printing GST  ************************************************************|\n");
    printf("|*************************************************************************************************************************|\n");
    printf("----------------------------------------------------------------------------------\n");
    printf("|Name\t\t|Size\t\t|Binding\t|Type-ptr\t|CBinding\t|\n");
    printf("----------------------------------------------------------------------------------\n");
    while(curr != NULL){
        char* tname = NULL;
        if(curr->type_entryy->c_type)
            tname = curr->type_entryy->c_type->name;
        else
            tname = curr->type_entryy->type_table->name;
        printf("|%s\t\t|%d\t\t|%d\t\t|%s-%d\t|%d\t\t|\n",curr->name,curr->size,curr->binding,tname, curr->type_entryy->ptr, curr->cbinding);
        print_array_int(curr->size_array);
        curr = curr->next;
    }
    printf("----------------------------------------------------------------------------------\n\n");
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

parameter* create_parameter(char* name, Type* type){
    parameter* node = (parameter *)malloc(sizeof(parameter));
    node->name = name;
    if(type->c_type)
        node->type_entryy = create_type_class(type->c_type);
    else
        node->type_entryy = create_type(type->type_table, type->ptr);
    node->next = NULL;
    return node;
}

parameter* add_parameter_to_list(parameter* lst, parameter* curr){
    //add to end of list
    if(lst == NULL)
        return curr;
    if(curr == NULL)
        return lst;
    lst->next = add_parameter_to_list(lst->next, curr);
    return lst;
}


int same_parameter_list(parameter* l1, parameter* l2){
    parameter* n1 = l1;
    parameter* n2 = l2;
    if(n1==NULL && n2==NULL)
        return 1;
    while(n1!=NULL && n2!=NULL){
        if(!n1->type_entryy && !n2->type_entryy && compare_type(n1->type_entryy, n2->type_entryy) == 0)
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

void print_param_list(parameter* ls){
    if(ls==NULL)
        return;
    printf("Param-%s-%d-ptr%d\n",ls->name,ls->type_entryy->type_table->name, ls->type_entryy->ptr);
    print_param_list(ls->next);
}

void free_param_list(parameter* ls){
    if(ls == NULL)
        return;
    if(ls->type_entryy)
        free(ls->type_entryy);
    free_param_list(ls->next);
    free(ls->name);
    free(ls);
}

/***********************Local Symbol Table Fns*********************/ 

Lsymbol* create_lsymbol(char* varname, Type* type, int binding, int cbinding, Lsymbol* next){
    if(get_variable_lst(varname, curr_lsymbol) != NULL){
        fprintf(stderr,"Variable redeclared:%s\n",varname);
        exit(1);
    }
    // printf("[DEBUG] Lsymbol created:%s\n", varname);
    Lsymbol* node = (Lsymbol*)malloc(sizeof(Lsymbol));
    node->size = 1;
    node->varname = strdup(varname);
    node->cbinding = cbinding;
    if(type->c_type){
        node->type_entryy = create_type_class(type->c_type);
        node->size+=1;
    }
    else
        node->type_entryy = create_type(type->type_table, type->ptr);
    node->binding = binding;
    node->next = next;
    node->size = 1;
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

Lsymbol* update_type_lsymbol_tb(Lsymbol* lst, Type* type){
    Lsymbol* curr = lst;
    while(curr!=NULL){
        curr->type_entryy->type_table = type->type_table;
        curr->type_entryy->c_type = type->c_type;
        curr->cbinding = -1;
        if(type->c_type){
            curr->cbinding = lst_binding++;
        }
        curr->type_entryy->ptr = curr->type_entryy->ptr || type->ptr;
        curr = curr->next;
    }
    return lst;
}

Lsymbol* correct_lsymbol_table(Lsymbol* lst){
    int req_binding = -3;
    Lsymbol* curr = lst;
    while(curr!=NULL){
        if(curr->binding < 0){
            int b = curr->binding;
            int bc = curr->cbinding;
            if(bc == b+1){
                curr->cbinding = req_binding--;
                curr->binding = req_binding--; 
            }
            else{
                curr->binding = req_binding--;
                curr->cbinding = curr->binding;
            }
        }
        curr = curr->next;
    }
    return lst;
}

Lsymbol* update_size_lsymbol_tb(Lsymbol* lst, Type* type){
    int sz = 0;
    if(type->c_type){
        sz = 2;
    }
    else{
        update_type_size(type->type_table);
    }
    
    Lsymbol* curr = lst;
    lst_binding = curr->binding;
    while(curr != NULL){
        curr->size = sz;
        if(curr->type_entryy->ptr == 1){
            curr->binding = lst_binding;
            curr->size = 1;
            lst_binding++;
        }
        else{
            curr->binding = lst_binding;
            if(curr->type_entryy->c_type){
                curr->cbinding = curr->binding+sz-1;
            }
            lst_binding+=sz;
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
    if(ls->type_entryy){
        free(ls->type_entryy);
    }
    // printf("[DEBUG]:Lsymbol freed:%s\n",ls->varname);
    free(ls->varname);
    free(ls);
}


Lsymbol* add_paramlist_lsymbol(parameter* param_ls, Lsymbol* tb, int binding){
    if(param_ls == NULL)
        return tb;
    int cbinding = binding;
    if(param_ls->type_entryy->c_type){
        binding--;
        cbinding = binding+1;
    }
    tb = add_paramlist_lsymbol(param_ls->next, tb, binding-1);
    tb = create_lsymbol(param_ls->name, param_ls->type_entryy, binding, cbinding, tb);
    return tb;
}


Lsymbol* add_self_lsymbol(Lsymbol* lst, ClassTable* cptr){
    // find lowest binding
    int binding = -2;
    Lsymbol* curr = lst;
    while(curr){
        binding = min(binding, curr->binding);
        curr = curr->next;
    }
    binding-=2; //TODO: Change acc to inheritance stack
    Type* type = create_type_class(cptr);
    char * name = "self";
    lst = create_lsymbol(name, type, binding,  binding+1, lst);
    return lst;
}

Lsymbol* lst_if_repeated(Lsymbol* lst){
    Lsymbol* curr = lst;
    while(curr != NULL){
        if(get_variable_lst(curr->varname, curr->next)!=NULL)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

FieldList* paraml_to_fieldl(parameter* param_ls){
    FieldList* fl = NULL;
    while(param_ls != NULL){
        // printf("[DEBUG] param:%s\n",param_ls->name);
        fl = field_list_add(fl, param_ls->name, param_ls->type_entryy->type_table);
        parameter* nxt = param_ls->next;
        free(param_ls->name);
        free(param_ls);
        param_ls = nxt;
    }
    return fl;
}

void print_lsymbol(){
    Lsymbol* curr = curr_lsymbol;

    printf("|*************************************************************************************************************************|\n");
    printf("|*********************************************  Printing LST  ************************************************************|\n");
    printf("|*************************************************************************************************************************|\n");
    printf("--------------------------------------------------\n");
    printf("|Name\t|Type\t|Size\t|Binding\t| CurrBinding\t|\n");
    printf("--------------------------------------------------\n");
    while(curr!=NULL){
        Type* type = curr->type_entryy;
        char* tname = NULL;
        if(type->c_type){
            tname = type->c_type->name;
        }
        else{
            tname = type->type_table->name;
        }
        printf("|%s\t|%s\t|%d\t|%d\t\t|%d\t\t|\n",curr->varname, tname, curr->size, curr->binding, curr->cbinding);
        curr = curr->next;
    }
    printf("---------------------------------------------------\n\n");

}