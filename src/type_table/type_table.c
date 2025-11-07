#include "type_table.h"

TypeTable* type_table = NULL;

void type_table_init(){
    // primitives
    type_table_add_primitive(TYPE_INT);
    type_table_add_primitive(TYPE_STR);
    // type_table_add_primitive(TYPE_INT_PTR);
    // type_table_add_primitive(TYPE_CHAR_PTR);
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
            name = strdup("str");
            break;
        case TYPE_BOOL:
            name = strdup("bool");
            break;
        case TYPE_VOID:
            name = strdup("void");
            break;
        case TYPE_INT_PTR:
            name = strdup("int_p");
            break;
        case TYPE_CHAR_PTR:
            name = strdup("str_p");
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

TypeTable* create_temp_type_table(char* name){
    if(type_table_get(name) != NULL){
        return type_table_get(name);
    }
    TypeTable* new_type = (TypeTable*)malloc(sizeof(TypeTable));
    new_type->name = name;
    return new_type;
}

TypeTable* type_table_add(char* name, int size, FieldList* fields){
    if(type_table_get(name) != NULL){
        fprintf(stderr, "Type redefined:%s",name);
        return NULL;
    }
    TypeTable* new_type = (TypeTable*)malloc(sizeof(TypeTable));
    new_type->size = size;
    new_type->name = strdup(name);
    // printf("[DEBUG] name: %s\n",new_type->name);
    new_type->fields = fields;
    new_type->next = type_table;
    type_table = new_type;
    update_type_size(type_table);
    return type_table;
}

void update_field_types(TypeTable* type){
    FieldList* f = type->fields;
    while(f){
        TypeTable* t = type_table_get(f->type->name);
        if(t == NULL){
            fprintf(stderr, "Type Error: Undeclared type used:%s\n",f->type->name);
            exit(1);
        }
        if(t!=f->type){
            free(f->type->name);
            free(f->type); // free the temporary type
            f->type = t;
        }
        f = f->next;
    }
}

void update_type_size(TypeTable* type){
    if(type == NULL)
        return;
    int cnt = 0;
    FieldList* f = type->fields;
    while(f){
        f = f->next;
        cnt++;
    }
    if(cnt>8){
        printf("Error: User defined variables cannot have size more than 8\n");
        exit(1);
    }
    type->size = cnt;
}

int get_type_size(TypeTable* type){
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
    curr->c_type =  NULL;
    return curr;
}

FieldList* field_list_add(FieldList* field_list, char* name, TypeTable* type){
    FieldList* curr = field_list;
    if(curr==NULL){
        return field_create(name, type);
    }
    while(curr->next != NULL){
        curr = curr->next;
    }
    curr->next = field_create(name, type);
    curr->next->field_id = curr->field_id+1;
    return field_list;
}

FieldList* field_list_join(FieldList* f1, FieldList* f2){
    if(f1 == NULL)
        return f2;
    FieldList* f_end = f1;
    while(f_end->next != NULL)
        f_end = f_end->next;
    f_end->next = f2;
    // f2->field_id = f_end->field_id+1;

    // f_end = f2;
    while(f_end->next != NULL){
        f_end->next->field_id = f_end->field_id+1;
        f_end = f_end->next;
    }

    return f1;
}

Type* create_type(TypeTable* type_table, int ptr){
    Type* curr = (Type*)malloc(sizeof(Type));
    curr->ptr = ptr;
    curr->type_table = type_table;
    curr->c_type = NULL;
    return curr;
}

Type* create_type_class(ClassTable* c){
    Type* curr = (Type*)malloc(sizeof(Type));
    curr->ptr = 0;
    curr->type_table = NULL;
    curr->c_type = c;
    return curr;
}

int compare_type(Type* t1, Type* t2){
    if(t1 == NULL || t2==NULL)
        return 0;
    if(t1->ptr != t2->ptr)
        return 0;
    if(t1->c_type != t2->c_type)
        return 0;
    if(t1->type_table != t2->type_table)
        return 0;
    return 1;
}
int compare_type_table(TypeTable* t1, TypeTable* t2){
    if(t1==t2)
        return 1;
    return 0;
}

int compare_class_type(Type* l, Type* r){
    if(!l->c_type || !r->c_type)
        return 0;
    // inherited return 1
    ClassTable* child = r->c_type;
    // Check if l->c_type is in the parent chain of r->c_type
    while(child){
        if(child== l->c_type)
            return 1;
        child = child->parent_ptr;
    }
    return 0;
}

void print_type_table(){
    TypeTable* curr = type_table;
    printf("|NAME\t\t|SZ\t\t|\n---------------------------------------------------------\n\n");
    while(curr){
        printf("%s\t\t%d\n", curr->name, curr->size);
        FieldList* f = curr->fields;
        printf("Fields\n");
        while(f){
            printf("|%d\t|%s\t\t|%s\t|\n",f->field_id, f->name, f->type->name);
            f = f->next;
        }
        printf("\n");
        curr = curr->next;
    }
}
////////////////////////////////////////////////////////////////////////
// Primitive Type Checks
int is_int(Type* type){
    if(!type || !compare_type_table(type->type_table, type_table_get("int")))
        return 0;
    if(type->ptr != 0)
        return 0;
    return 1;
}
int is_int_ptr(Type* type){
    if(!type || !compare_type_table(type->type_table, type_table_get("int")))
        return 0;
    if(type->ptr != 1)
        return 0;
    return 1;
}

int is_str(Type* type){
    if(!type || compare_type_table(type->type_table, type_table_get("str")) != 1)
        return 0;
    if(type->ptr != 0)
        return 0;
    return 1;
}
int is_str_ptr(Type* type){
    if(!type || compare_type_table(type->type_table, type_table_get("str")) != 1)
        return 0;
    if(type->ptr != 1)
        return 0;
    return 1;
}

int is_bool(Type* type){
    if(!type || compare_type_table(type->type_table, type_table_get("bool")) != 1)
        return 0;
    if(type->ptr != 0)
        return 0;
    return 1;
}

int is_null(Type* type){
    if(!type || compare_type_table(type->type_table, type_table_get("null")) != 1)
        return 0;
    return 1;
}

int is_void(Type* type){
    if(!type || compare_type_table(type->type_table, type_table_get("void")) != 1)
        return 0;
    return 1;
}

int is_tuple(Type* type){
    if(!type || (type->c_type) || strncmp("tuple-",type->type_table->name,6)!=0)
        return 0;
    return 1;
}