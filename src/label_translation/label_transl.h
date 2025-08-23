#ifndef LABEL_TRANSL_HEADER_FILE
#define LABEL_TRANSL_HEADER_FILE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct label_address{
    char* label;
    int address;
    struct label_address* next;
} label_node;

/**
 * Function: create_label_node
 * --------------------------
 * Adds the new node to the head of the list
 * 
 * Input :
 * - char * label - will be directly assigned to the node
 * - int address - address of the label
 * 
 * Output:
 * - node
 */
label_node* create_label_node(char* label, int address);

/**
 * Function: get_address
 * ------------------------
 * Searches the linked list to find the label
 * 
 * Input:
 * - char* label - The label whose address is to be searched
 * 
 * 
 * Output:
 * - >0 the address stored corresponding to the label
 * - -1 if label not found
 */
int get_address(char* label);

/**
 * Function: print_node
 * -----------------------
 * To print the linked list nodes one by one
 */
void print_node();

#endif