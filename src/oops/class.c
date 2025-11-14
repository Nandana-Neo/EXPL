#include "class.h"

static ClassTable* g_class_table = NULL;
ClassTable* curr_class = NULL;

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

ClassTable* get_global_ct(){
    return g_class_table;
}

void ct_install_inherited(ClassTable* cptr){
    ClassTable* parent = cptr->parent_ptr;
    if(parent == NULL){
        fprintf(stderr,"ERROR(ct_install_inherited): Parent class not found\n");
        exit(1);
    }
    // copy fields
    FieldList* parent_fields = parent->fields;
    while(parent_fields != NULL){
        if(parent_fields->private!=1){
            // install only if it is not private field
            class_f_install(cptr, parent_fields->c_type, parent_fields->type, parent_fields->name);
        }
        parent_fields = parent_fields->next;
    }

    // copy methods
    MethodList* parent_methods = parent->methods;
    while(parent_methods != NULL){
        class_m_install(cptr, parent_methods->name, parent_methods->type, parent_methods->param_list, parent_methods->f_label);
        parent_methods = parent_methods->next;
    }
}

ClassTable* ct_get(char* name){
    if(name == NULL)
        return NULL;
    ClassTable* curr = g_class_table;
    while(curr != NULL){
        if(strcmp(curr->name,name)==0){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

FieldList* class_f_install(ClassTable* curr, ClassTable* cptr, TypeTable* type, char* name){
    if(curr == NULL){
        fprintf(stderr,"ERROR(class_f_install): Classptr is null\n");
        return NULL;
    }
    curr->fields = field_list_add(curr->fields, name, type);
    curr->field_cnt++;
    if(curr->field_cnt > 8){
        fprintf(stderr, "ERROR: Class %s has more than 8 fields\n", curr->name);
        exit(1);
    }
    FieldList* f = class_f_get(curr, name);
    f->c_type = cptr;
    return f;
}

void class_m_install(ClassTable* cptr, char* name, TypeTable* type, parameter* param_list, int f_label){
    if(cptr == NULL){
        fprintf(stderr,"ERROR(class_m_install): Classptr is null\n");
        return;
    }
    cptr->methods = method_list_add(cptr->methods, name, f_label, type, param_list);
    cptr->method_cnt++;

    if(cptr->method_cnt > 8){
        fprintf(stderr, "ERROR: Class %s has more than 8 methods\n", cptr->name);
        exit(1);
    }
    
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


MethodList* class_m_get(ClassTable* cptr, char* f_name, parameter* param_list){
    if(cptr == NULL){
        printf("WARNING: Empty class tried to access\n");
        return NULL;
    }
    MethodList* m = cptr->methods;
    while(m!=NULL){
        if(strcmp(m->name, f_name) == 0){
            printf("Checked:%s\n", f_name);
            print_param_list(m->param_list);
            print_param_list(param_list);
            if(same_parameter_list(m->param_list, param_list) == 1){
                return m;
            }
        }
        m = m->next;
    }
    return NULL;
}

ListOfMethods* class_m_get_lst(ClassTable* cptr, char* f_name){
    ListOfMethods* lm = NULL;
    if(cptr == NULL){
        printf("WARNING: Empty class tried to access\n");
        return lm;
    }
    MethodList* m = cptr->methods;
    while(m!=NULL){
        if(strcmp(m->name, f_name) == 0){
            lm = list_methods_add(lm, m);
        }
        m = m->next;
    }
    return lm;
}

MethodList* class_m_get_single(ListOfMethods* lm, parameter* param_list){
    ListOfMethods* curr = lm;
    while(curr != NULL){
        MethodList* m = curr->method;
        if(same_parameter_list(m->param_list, param_list) == 1){
            return m;
        }
        curr = curr->next;
    }
    return NULL;
    
}

int same_type_parameter_list(parameter* l1, parameter* l2){
    if(l1 == NULL && l2 == NULL)
        return 1;
    if(l1==NULL || l2 == NULL)
        return 0;
    if(compare_type(l1->type_entryy, l2->type_entryy) != 1)
        return 0;
    return same_type_parameter_list(l1->next, l2->next);
}

MethodList* class_m_get_from_list_and_type(ListOfMethods* lm, parameter* param_lst_type){
    ListOfMethods* curr = lm;
    while(curr != NULL){
        MethodList* m = curr->method;
        if(same_type_parameter_list(m->param_list, param_lst_type) == 1){
            return m;
        }
        curr = curr->next;
    }
    return NULL;
}

FieldList* class_f_get(ClassTable* cptr, char* v_name){
    if(cptr == NULL){
        printf("WARNING: Empty class tried to access\n");
        return NULL;
    }
    FieldList* f = cptr->fields;
    while(f){
        if(strcmp(f->name, v_name) == 0){
            return f;
        }
        f = f->next;
    }
    return NULL;
}


int get_class_vft_baseptr(int index){
    return SP+(index*8);
}

ListOfMethods* list_methods_add(ListOfMethods* lm, MethodList* m){
    ListOfMethods* new_lm = (ListOfMethods*)malloc(sizeof(ListOfMethods));
    new_lm->method = m;
    new_lm->next = NULL;
    if(lm == NULL){
        return new_lm;
    }
    ListOfMethods* curr = lm;
    while(curr->next != NULL){
        curr = curr->next;
    }
    curr->next = new_lm;
    return lm;
}

void free_list_of_methods(ListOfMethods* lm){
    ListOfMethods* curr = lm;
    while(curr != NULL){
        ListOfMethods* temp = curr;
        curr = curr->next;
        free(temp);
    }
}