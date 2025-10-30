#ifndef OOPS_CLASS_HEADER_FILE
#define OOPS_CLASS_HEADER_FILE

#include "../node/type_node.h"
#include "../symbol_table/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

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
void class_f_install(ClassTable* cptr, TypeTable* type, char* name);

/**
 * class_m_install()
 * -------------------
 * Installs the method into the given class table entry
 */
void class_m_install(ClassTable* cptr, char* name, TypeTable* type, parameter* param_list);

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
 */
MethodList* class_m_get(ClassTable* cptr, char* f_name);

/**
 * class_f_get()
 * --------------
 * Search through the fields of the current class using cptr that is being parsed and return ptr to the entry in the list with the variable name as v_name
 * Return NULL if entry is not found
 */
FieldList* class_f_get(ClassTable* cptr, char* v_name);

#endif