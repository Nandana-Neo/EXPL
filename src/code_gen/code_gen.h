#ifndef CODE_GEN_HEADER_FILE
#define CODE_GEN_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include "../node/type_node.h"
#include "../node/ast_node.h"
#include "../symbol_table/symbol_table.h"

/**
 * Function: get_reg
 * -----------------
 * Input : None
 * 
 * Output : 
 * - Returns the available register
 * - Exits with error 1 if no available register
 */
int get_reg();

/**
 * Function: free_reg
 * ------------------
 * Input: None
 * 
 * Output: 
 * - Returns 0 is successfully freed highest val register
 * - Exits with error 1 if no register to free
 */
int free_reg();

/**
 * Function: get_label()
 * ----------------------
 * Input: None
 * 
 * Output: 
 * - Returns an integer that represents the available label
 */
int get_label();


/**
 * Function : code_gen
 * --------------------
 * Generate assembly code corresponding to AST node
 * Note: It will print newline at the end
 * 
 * Input:
 * - node - node
 * - fp - file pointer fp
 * - start_label - start label of the loop containing this node (for continue)
 * - end_label - end label of the loop containing this node (for break)
*/
loc_and_val* code_gen(tnode* node, FILE * fp, int start_label, int end_label);

/**Function: code_gen_ARG
 * --------------------------
 * Generate code for the arguments to a function as well as push the value of the argument into the stack
 */
loc_and_val* code_gen_ARG(tnode* node, FILE* fp, int start_label, int end_label);

/**Function: code_gen_FN_CALL
 * -------------------------------
 * Generate code for a caller fn node
 */
loc_and_val* code_gen_FN_CALL(tnode* node, FILE* fp, int start_label, int end_label);

/**Function: push_local_decl 
 * --------------------------
 * Generates code to push the local declarations in the lst specified into the stack
 * Checks by binding>0
*/
void push_local_decl(Lsymbol* lst, FILE* fp);

/**Function: pop_local_decl 
 * --------------------------
 * Generates code to pop the local declarations in the lst specified into the stack
 * Checks by binding>0
*/
void pop_local_decl(Lsymbol* lst, FILE* fp);

/**
 * Function: code_gen_fn_begin 
 * ------------------------------
 * Generates code to push BP, update BP to SP and push local decl
 */
void code_gen_fn_begin(FILE* fp);

/**
 * Function: code_gen_fn_end
 * ---------------------------
 * Generates code to pop local decl, pop BP and RET
 */
void code_gen_fn_end(FILE* fp);

/**Function: code_gen_fn
 * ---------------------
 * Generates code for the function from callee side
 */
loc_and_val* code_gen_fn(tnode* node, FILE* fp);

/**
 * Function: code_gen_RET
 * -------------------------
 * Generates code for a return exp node
 */
loc_and_val* code_gen_RET(tnode* node, FILE* fp, int start_label, int end_label);

/**
 * Function : code_gen_ID
 * ----------------------
 * Generates code for ID leaf node
 * 
 * Output
 * - Returns reg no storing ID
 * 
 */
loc_and_val* code_gen_ID(tnode *node, FILE* fp);

/**
 * Function : code_gen_VAL
 * ----------------------
 * Generates code for NUM/STR leaf node
 * 
 * Output
 * - Returns reg no storing NUM or the STR
 * 
 */
loc_and_val* code_gen_VAL(tnode *node, FILE* fp);

/**
 * Function : code_gen_OP
 * ----------------------
 * Generates code for operator nodes: +, -, *, /, =, Connector
 * 
 * Output
 * - Returns reg no storing the output of the operation except for 
 * = and connector which will return -1 and free the left and right regs
 * 
 */
loc_and_val* code_gen_OP(tnode* node, FILE* fp);


/**
 * Function : code_gen_READ
 * ----------------------
 * Generates code for reading input to a variable from the console and to store the value into the memory location 
 * 
 * Output
 * - Returns reg no storing the output of read operation (SUCCESS / FAILURE)
 * 
 */
loc_and_val* code_gen_READ(tnode* node, FILE *fp);

/**
 * Function : code_gen_WRITE
 * ----------------------
 * Generates code for writing output onto the console
 * 
 * Output
 * - Returns reg no storing the output of write operation (SUCCESS / FAILURE)
 * 
 */
loc_and_val* code_gen_WRITE(tnode* node, FILE *fp);

/**
 * Function: code_gen_IF
 * ----------------------
 * Generates code for IF-THEN and IF-THEN-ELSE statements
 * Returns -1 since it is a statement
 * 
 * Input:
 * - node - node
 * - fp - file pointer fp
 * - start_label - start label of the loop containing this node (for continue)
 * - end_label - end label of the loop containing this node (for break)
 */
loc_and_val* code_gen_IF(tnode* node, FILE* fp, int start_label, int end_label);

/**
 * Function: code_gen_DO_WHILE
 * ------------------------
 * Generates code for DO-WHILE statement
 * Returns -1 since it is a statement
 */
loc_and_val* code_gen_DO_WHILE(tnode* node, FILE* fp);

/**
 * Function: code_gen_REPEAT
 * ------------------------
 * Generates code for REPEAT-UNTIL statement
 * Returns -1 since it is a statement
 */
loc_and_val* code_gen_REPEAT(tnode* node, FILE* fp);

/**
 * Function: code_gen_WHILE
 * ------------------------
 * Generates code for WHILE-DO statement
 * Returns -1 since it is a statement
 */
loc_and_val* code_gen_WHILE(tnode* node, FILE* fp);

/**
 * Function: code_gen_BREAK
 * ------------------------
 * Code for jump to end_label
 * 
 * Inputs:
 * - node
 * - fp
 * - end_label: if not in a loop and is to be ignored, -1. Else enter >= 0
 */
loc_and_val* code_gen_BREAK(tnode* node, FILE* fp, int end_label);

/**
 * Function: code_gen_CONTINUE
 * ------------------------
 * Code for jump to start_label
 * 
 * Inputs:
 * - node
 * - fp
 * - end_label: if not in a loop and is to be ignored, -1. Else enter >= 0
 */
loc_and_val* code_gen_CONTINUE(tnode* node, FILE* fp, int start_label);

/**
 * Function : code_gen_CONN
 * --------------------
 * Generate assembly code corresponding to AST node
 * Note: It will print newline at the end
 * 
 * Input:
 * - node - node
 * - fp - file pointer fp
 * - start_label - start label of the loop containing this node (for continue)
 * - end_label - end label of the loop containing this node (for break)
*/
loc_and_val* code_gen_CONN(tnode* node, FILE* fp, int start_label, int end_label);

loc_and_val* code_gen_INDEX(tnode* node, FILE* fp, array* size_array_ptr);

loc_and_val* code_gen_ARR(tnode* node, FILE* fp);

loc_and_val* code_gen_ADDR_OF(tnode* node, FILE* fp);

loc_and_val* code_gen_VAL_AT(tnode* node, FILE* fp);

void code_gen_final(FILE * fp);

void code_gen_start(FILE * fp);
void code_gen_SP_init(FILE* fp);

int evaluate(tnode* node);

loc_and_val* create_gen_node(int loc, int val);

void free_gen_node(loc_and_val* node);

#endif