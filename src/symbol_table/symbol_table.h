#ifndef SYMBOL_TABLE_HEADER_FILE
#define SYMBOL_TABLE_HEADER_FILE

#include <stdlib.h>
#include <string.h>
#include "../node/ast_node.h"
#include "../node/decl_node.h"

typedef struct Gsymbol {
    char* name;       // name of the variable
    int type;         // type of the variable - INT or STR
    int size;         // size of the type of the variable - default(1)
    int binding;      // stores the static memory address allocated to the variable
    struct Gsymbol *next;
} Gsymbol;

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
 * 
 * Output:
 * - Gsymbol node   // symbol table entry of the variable assigned
 */
Gsymbol* add_variable(char* name, VarType type);

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

/**Function: create_entries 
 * ---------------------------
 * Registers the variables in the list ls with the type 'type'
 * into the symbol table. Also frees the list completely including the variable names
 * 
 * Input: 
 * - decl_node * ls - list of the decl_nodes
 * - VarType type - type of the varaibles to be assigned
 * 
 * Output:
 * void
 */

void create_entries(decl_node * ls, VarType type);

#endif