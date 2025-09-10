#include "symbol_table.h"

int SP = 4096;

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
    node->next = symbol_table;
    node->size_array = NULL;
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


void create_entries(decl_node * ls, VarType type){
    decl_node * curr = ls;
    while(curr != NULL){
        char* varname = strdup(curr->varname);
        char* lengths = strdup(curr->varname);
        int dimension = 0;
        sscanf(curr->varname,"%s %d %s",varname,&dimension,lengths);  //for arrays: "ID 2 10,10" for the case of ID[10][10]
        Gsymbol* node = add_variable(varname, curr->size, type);
        printf("[DEBUG]VAR: %s", varname);
        array* head = NULL;
        if(dimension>=1){
            //array
            char* token = strtok(lengths, ",");
            int elem;
            while(token != NULL){
                elem = atoi(token);
                head = add_array_node(head, elem);
                token = strtok(NULL,",");
            }
        }
        node->size_array = head;
        decl_node * prev = curr;
        curr = curr->next;
        free_decl_node(prev);
    }
}

void print_st(){
    Gsymbol * curr = symbol_table;
    while(curr != NULL){
        printf("%s-%d-%d\n",curr->name,curr->size,curr->binding);
        curr = curr->next;
    }
}

array* add_array_node(array* arr,int val){
    array* node = (array *)malloc(sizeof(array));
    node->val = val;
    node->nxt = NULL;
    array* hd = arr;
    if(hd == NULL)
        return node;
    while(hd->nxt != NULL){
        hd = hd->nxt;
    }
    hd->nxt = node;
    return hd;
}