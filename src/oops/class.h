#ifndef OOPS_CLASS_HEADER_FILE
#define OOPS_CLASS_HEADER_FILE

#include "../node/type_node.h"
#include "../symbol_table/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>


/**
 * Variable used for understanding which class def currently in
 */
extern ClassTable* curr_class;

/**
 * Returns the global class table starting pointer
 */
ClassTable* get_global_ct();

/**
 * FN: ct_install()
 * --------------
 * Input:
 * - name - name of the class
 * - parent_class_name - name of the parent class if extension of it
 * ----------------
 * Creates a class table entry of given 'name' and extends the fields and the methods of parent class and returns a pointer to the newly created class entry.
 */
ClassTable* ct_install(char* name, char* parent_class_name);


/**
 * FN: ct_install_inherited()
 * --------------------------
 * Installs the fields and methods of the parent class into the given class table entry 'cptr'
 */
void ct_install_inherited(ClassTable* cptr);

/**
 * FN: ct_get()
 * ------------------
 * Search for a class table entry with the given 'name', if exists, return pointer to class table entry else return NULL.
 */
ClassTable* ct_get(char* name);

/**
 * class_f_install()
 * -----------------------
 * Installs the field into the given class table entry
 */
void class_f_install(ClassTable* curr, ClassTable* cptr, TypeTable* type, char* name);

/**
 * class_m_install()
 * -------------------
 * Installs the method into the given class table entry
 */
void class_m_install(ClassTable* cptr, char* name, TypeTable* type, parameter* param_list, int f_label);

/**
 * create_method()
 * -----------------
 * create method with name, f_label, type, param_list
 */
MethodList* create_method(char* name, int f_label, TypeTable* type, parameter* param_list);

/**
 * method_list_add
 * ---------------
 * Adds the new method with name to the end of m1
 */
MethodList* method_list_add(MethodList* m1, char* name, int f_label, TypeTable* type, parameter* param_list);
 
/**
 * method_list_join
 * -----------------
 * Joins method list m2 to end of m1
 */
MethodList* method_list_join(MethodList* m1, MethodList* m2);

/**
 * class_m_get()
 * ---------------
 * Search through the methods MethodList of the class using cptr that is being parsed and return pointer to the entry in the list with function name as f_name. 
 * Return NULL if entry not found
 * 
 * Returns list of methods having the name f_name
 */
MethodList* class_m_get(ClassTable* cptr, char* f_name, parameter* param_list);

ListOfMethods* class_m_get_lst(ClassTable* cptr, char* f_name);

/**
 * class_m_get_single()
 * ---------------------
 * From the list of methods lm, return the method which has the same parameter list as param_list
 * Return NULL if no such method found
 */
MethodList* class_m_get_single(ListOfMethods* lm, parameter* param_list);

int same_type_parameter_list(parameter* l1, parameter* l2);

MethodList* class_m_get_from_list_and_type(ListOfMethods* lm, parameter* param_lst_type);
/**
 * class_f_get()
 * --------------
 * Search through the fields of the current class using cptr that is being parsed and return ptr to the entry in the list with the variable name as v_name
 * Return NULL if entry is not found
 */
FieldList* class_f_get(ClassTable* cptr, char* v_name);

/**
 * get_class_vft_baseptr
 * ------------------
 * Returns the address of the vft pointing to the class specified by index
 * That is,
 * Return 8*index + SP
 * 
 * - index starts from 0
 */
int get_class_vft_baseptr(int index);

/**
 * list_methods_add
 * -----------------
 * Adds method m to the end of ListMethods lm
 */
ListOfMethods* list_methods_add(ListOfMethods* lm, MethodList* m);

void free_list_of_methods(ListOfMethods* lm);

#endif