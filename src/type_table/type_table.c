#include "type_table.h"

TypeTable* type_table = NULL;

void type_table_init(){
    // primitives
    type_table_add_primitive(TYPE_INT);
    type_table_add_primitive(TYPE_STR);
    type_table_add_primitive(TYPE_BOOL);
    // special values
    type_table_add_primitive(TYPE_NULL);
    type_table_add_primitive(TYPE_VOID);
}

TypeTable* type_table_add_primitive(VarType type){
    char* name;
    switch(type){
        case TYPE_INT:
            name = strdup("int");
            break;
        case TYPE_STR:
        case TYPE_CHAR:
            name = strdup("char");
            break;
        case TYPE_BOOL:
            name = strdup("bool");
            break;
        case TYPE_VOID:
            name = strdup("void");
            break;
        case TYPE_NULL:
            name = strdup("null");
            break;
        default:
            fprintf(stderr,"Type not found\n");
            exit(1);
    }
    return type_table_add(name, 1, NULL);
}

TypeTable* type_table_get(char* name){
    TypeTable* curr = type_table;
    while(curr!=NULL){
        if(strcmp(curr->name, name) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

TypeTable* type_table_add(char* name, int size, FieldList* fields){
    if(type_table_get(name) != NULL){
        fprintf(stderr, "Type redefined:%s",name);
        return NULL;
    }
    TypeTable* new_type = (TypeTable*)malloc(sizeof(TypeTable));
    new_type->size = size;
    new_type->name = name;
    new_type->fields = fields;
    new_type->next = type_table;
    type_table = new_type;
    return type_table;
}

int get_size(TypeTable* type){
    return type->size;
}

FieldList* field_list_get(char* name, TypeTable* type){
    FieldList* curr = type->fields;
    while(curr != NULL){
        if(strcmp(name, curr->name) == 0)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

FieldList* field_create(char* name, TypeTable* type){
    FieldList* curr = (FieldList*)malloc(sizeof(FieldList));
    curr->name = strdup(name);
    curr->field_id = 0;
    curr->type = type;
    curr->next = NULL;
    return curr;
}

void field_list_add(FieldList* field_list, char* name, TypeTable* type){
    FieldList* curr = field_list;
    if(curr==NULL){
        return field_create(name, type);
    }
    while(curr->next != NULL){
        curr = curr->next;
    }
    curr->next = field_create(name, type);
    curr->next->field_id = curr->field_id+1;
}
