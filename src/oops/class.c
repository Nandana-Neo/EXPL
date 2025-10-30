#include "class.h"

static ClassTable* g_class_table = NULL;


ClassTable* ct_install(char* name, char* parent_class_name){
    if(ct_get(name) != NULL){
        return ct_get(name);
    }
    ClassTable* new_class = (ClassTable*)malloc(sizeof(ClassTable));
    new_class->field_cnt = 0;
    new_class->method_cnt = 0;
    new_class->fields = NULL;
    new_class->methods = NULL;

    new_class->name = strdup(name);
    new_class->parent_ptr = ct_get(parent_class_name);

    new_class->class_id = 0;
    new_class->next = NULL;

    ClassTable* curr_class = g_class_table;
    if(curr_class == NULL){
        return g_class_table = new_class;
    }

    while(curr_class->next != NULL){
        curr_class = curr_class->next;
    }
    new_class->class_id = curr_class->class_id+1;
    curr_class->next = new_class;

    return new_class;
}

ClassTable* ct_get(char* name){
    ClassTable* curr = g_class_table;
    while(curr != NULL){
        if(strcmp(curr->name,name)==0){
            return curr;
        }
    }
    return NULL;
}

void class_f_install(ClassTable* cptr, TypeTable* type, char* name){
    if(cptr == NULL){
        fprintf(stderr,"ERROR(class_f_install): Classptr is null\n");
        return;
    }
    cptr->fields = field_list_add(cptr->fields, name, type);
    cptr->field_cnt++;
}

void class_m_install(ClassTable* cptr, char* name, TypeTable* type, parameter* param_list){
    if(cptr == NULL){
        fprintf(stderr,"ERROR(class_m_install): Classptr is null\n");
        return;
    }
    cptr->methods = method_list_add(cptr->methods, name, get_f_label(), type, param_list);
    cptr->method_cnt++;
    
}

MethodList* create_method(char* name, int f_label, TypeTable* type, parameter* param_list){
    
    MethodList* new_m = (MethodList*)malloc(sizeof(MethodList));
    new_m->func_id = 0;
    new_m->f_label = f_label;
    new_m->name = strdup(name);
    new_m->param_list = param_list;
    new_m->type = type;
    new_m->next = NULL;
    return new_m;
}

MethodList* method_list_add(MethodList* m1, char* name, int f_label, TypeTable* type, parameter* param_list){
    MethodList* m = m1;
    if(m == NULL)
        return create_method(name, f_label, type, param_list);
    while(m->next != NULL){
        m = m->next;
    }
    m->next = create_method(name, f_label, type, param_list);
    m->next->func_id = m->func_id + 1;
    return m1;
}

MethodList* method_list_join(MethodList* m1, MethodList* m2){
    if(m1==NULL){
        return m2;
    }
    MethodList* m_end = m1;
    while(m_end->next != NULL){
        m_end = m_end->next;
    }
    m_end->next = m2;

    while(m_end->next != NULL){
        m_end->next->func_id = m_end->func_id + 1;
        m_end = m_end->next;
    }

    return m1;
}

MethodList* class_m_get(ClassTable* cptr, char* f_name){
    MethodList* m = cptr->methods;
    while(m!=NULL){
        if(strcmp(m->name, f_name) == 0){
            return m;
        }
        m = m->next;
    }
    return NULL;
}

FieldList* class_f_get(ClassTable* cptr, char* v_name){
    FieldList* f = cptr->fields;
    while(f){
        if(strcmp(f->name, v_name) == 0){
            return f;
        }
        f = f->next;
    }
    return NULL;
}
