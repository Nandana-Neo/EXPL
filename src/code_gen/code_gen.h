#ifndef CODE_GEN_HEADER_FILE
#define CODE_GEN_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>

/**
 * Function: getReg
 * -----------------
 * Input : None
 * 
 * Output : 
 * - Returns the available register
 * - Exits with error 1 if no available register
 */
int getReg();

/**
 * Function: freeReg
 * ------------------
 * Input: None
 * 
 * Output: 
 * - Returns 0 is successfully freed highest val register
 * - Exits with error 1 if no register to free
 */
int freeReg();

#endif