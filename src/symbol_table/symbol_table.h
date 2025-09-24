#ifndef SYMBOL_TABLE_HEADER_FILE
#define SYMBOL_TABLE_HEADER_FILE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../node/type_node.h"

// Stores the address to the symbol 
static Gsymbol * symbol_table = NULL;

extern int SP;   // points to the free slot at top of stack


/**
 * Function: add_variable
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
Gsymbol* add_variable(char* name, int size, VarType type);

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

#endif