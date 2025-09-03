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
int code_gen(tnode* node, FILE * fp, int start_label, int end_label);

/**
 * Function : code_gen_ID
 * ----------------------
 * Generates code for ID leaf node
 * 
 * Output
 * - Returns reg no storing ID
 * 
 */
int code_gen_ID(tnode *node, FILE* fp);

/**
 * Function : code_gen_VAL
 * ----------------------
 * Generates code for NUM/STR leaf node
 * 
 * Output
 * - Returns reg no storing NUM or the STR
 * 
 */
int code_gen_VAL(tnode *node, FILE* fp);

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
int code_gen_OP(tnode* node, FILE* fp);


/**
 * Function : code_gen_READ
 * ----------------------
 * Generates code for reading input to a variable from the console and to store the value into the memory location 
 * 
 * Output
 * - Returns reg no storing the output of read operation (SUCCESS / FAILURE)
 * 
 */
int code_gen_READ(tnode* node, FILE *fp);

/**
 * Function : code_gen_WRITE
 * ----------------------
 * Generates code for writing output onto the console
 * 
 * Output
 * - Returns reg no storing the output of write operation (SUCCESS / FAILURE)
 * 
 */
int code_gen_WRITE(tnode* node, FILE *fp);

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
int code_gen_IF(tnode* node, FILE* fp, int start_label, int end_label);

/**
 * Function: code_gen_DO_WHILE
 * ------------------------
 * Generates code for DO-WHILE statement
 * Returns -1 since it is a statement
 */
int code_gen_DO_WHILE(tnode* node, FILE* fp);

/**
 * Function: code_gen_REPEAT
 * ------------------------
 * Generates code for REPEAT-UNTIL statement
 * Returns -1 since it is a statement
 */
int code_gen_REPEAT(tnode* node, FILE* fp);

/**
 * Function: code_gen_WHILE
 * ------------------------
 * Generates code for WHILE-DO statement
 * Returns -1 since it is a statement
 */
int code_gen_WHILE(tnode* node, FILE* fp);

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
int code_gen_BREAK(tnode* node, FILE* fp, int end_label);

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
int code_gen_CONTINUE(tnode* node, FILE* fp, int start_label);

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
int code_gen_CONN(tnode* node, FILE* fp, int start_label, int end_label);

void code_gen_final(FILE * fp);

void code_gen_start(FILE * fp);

int evaluate(tnode* node);

#endif