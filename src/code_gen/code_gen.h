#ifndef CODE_GEN_HEADER_FILE
#define CODE_GEN_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include "../exptree/exptree.h"

#define SP 4096
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
 * Function : code_gen
 * --------------------
 * Generate assembly code corresponding to AST node
 * Note: It will print newline at the end
*/
int code_gen(tnode* node, FILE * fp);

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
 * Function : code_gen_NUM
 * ----------------------
 * Generates code for NUM leaf node
 * 
 * Output
 * - Returns reg no storing NUM
 * 
 */
int code_gen_NUM(tnode *node, FILE* fp);

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

void code_gen_final(FILE * fp);

void code_gen_start(FILE * fp);

#endif