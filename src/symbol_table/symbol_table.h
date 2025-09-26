#ifndef SYMBOL_TABLE_HEADER_FILE
#define SYMBOL_TABLE_HEADER_FILE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../node/type_node.h"

// Stores the address to the symbol 
static Gsymbol * symbol_table = NULL;

extern int SP;   // points to the free slot at top of stack

int get_f_label();

/**
 * Function: add_variable_to_symbol
 * ------------------------
 * Registers the variable to the symbol table and returns the head of the symbol table.
 * name is re-allocated. Hence, free the input after usage manually
 * 
 * Input: 
 * - name: char *    // name of the variable
 * - type: int       // type of the variable
 * - size: int       // size allocated to var
 * 
 * Output:
 * - Gsymbol node   // symbol table entry of the variable assigned
 */
Gsymbol* add_variable_to_symbol(char* name, int size, VarType type);

/**
 * Function: add_fn
 * ------------------
 * Adds a fn declaration to the symbol table
 * 
 * Input:
 * - name: char *
 * - return_type: VarType
 * - param_list: parameter *
 */
Gsymbol* add_fn_to_symbol(char* name, VarType return_type, parameter* param_list);

/**
 * Function: get_variable
 * ------------------------
 * 
 * Input:
 * - name : char*   // name of the variable
 * 
 * Output:
 * - Gsymbol node   // Symbol table entry of the variable
 * Returns NULL if variable is not in the table
 */
Gsymbol* get_variable(char* name);

/**
 * Function: add_array_to_symbol
 * 
 * Adds the array node to the symbol tree along with moving the location to the array node
 */
Gsymbol* add_array_to_symbol(FILE* fp, char* varname,array* array_sz, VarType type, int sz);

void print_st();

void print_array_int(array * arr);

array* add_array_node(array* arr,int val);

parameter* create_parameter(char* name, VarType type);

parameter* add_parameter_to_list(parameter* lst, parameter* curr);
/**Function: int same_parameter_list(parameter* l1, parameter* l2)
 * ----------------------------------------------------------------
 * Checks if the contents of the lists are same.
 * Returns 1 if same,
 * else 0
 */
int same_parameter_list(parameter* l1, parameter* l2);

/******************LOCAL SYMBOL TABLE********************/ 
typedef struct Lsymbol{
    char* varname;
    VarType type;
    int binding;
    struct Lsymbol* next;
} Lsymbol;

/**
 * Function: create_lsymbol()
 * -------------------------------
 * Input:
 * - varname: char*
 * - type:  VarType -give TYPE_INT or TYPE_INT_PTR for default
 * - binding: int -> give -1 for default
 * - next: Lsymbol* -> give NULL for default
 * 
 * The varname is re-assigned. Hence, can free the varname outside the fn
 */
Lsymbol* create_lsymbol(char* varname, VarType type, int binding, Lsymbol* next);

/**
 * Function: connect_lsymbol()
 * ----------------------------
 * Input:
 * - lst1: Lsymbol*
 * - lst2: Lsymbol*
 * Output:
 * Joins lst2 to the end of lst1 and returns the head of lst1
 */
Lsymbol* connect_lsymbol(Lsymbol* lst1, Lsymbol* lst2);

/**
 * Function: update_type_lsymbol_tb(Lsymbol* lst, Vartype type)
 * --------------------------------------
 * Input: 
 * - lst: Lsymbol*
 * - type: VarType
 * Output:
 * Updates the type of all the lst variables from default int or int_ptr to type or type_ptr
 */
Lsymbol* update_type_lsymbol_tb(Lsymbol* lst, VarType type);

#endif