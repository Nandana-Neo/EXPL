#ifndef TYPE_TABLE_HEADER_FILE
#define TYPE_TABLE_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_NONE = -1,   // not ID node
    TYPE_INT  = 0,
    TYPE_CHAR = 1,
    TYPE_BOOL = 2,
    TYPE_STR = 3,
    TYPE_INT_PTR,
    TYPE_CHAR_PTR,
    TYPE_VOID,
    TYPE_NULL,
    TYPE_CUSTOM
} VarType;

typedef struct FieldList{
  char *name;              //name of the field
  int field_id;          //the position of the field in the field list
  struct TypeTable *type;  //pointer to type table entry of the field's type
  struct FieldList *next;  //pointer to the next field
}FieldList;

typedef struct TypeTable{
    char *name;                 //type name
    int size;                   //size of the type
    struct FieldList *fields;   //pointer to the head of fields list
    struct TypeTable *next;     // pointer to the next type table entry
} TypeTable;

extern TypeTable* type_table;

/**
 * Function: type_table_init
 * ---------------------------
 * Function to initialise the type table entries with primitive types (int,str) and special entries_(boolean,null,void).
 */
void type_table_init();


/**
 * Search through the type table and return pointer to type table entry of type 'name'. 
 * Returns NULL if entry is not found.
 */
TypeTable* type_table_get(char *name);

/**
 * Creates a type table entry for the (user defined) type of 'name' with given 'fields' and returns the pointer to the type table entry. 
 * The field list must specify the field index, type and name of each field. 
 * 
 * Returns NULL upon failure. This routine is invoked when the compiler encounters a type definition in the source program.
 */
TypeTable* type_table_add(char* name, int size, FieldList* fields);

/**
 * Add primitive data type into type table
 * Returns NULL if failed
 */
TypeTable* type_table_add_primitive(VarType type);

/**
 * Searches for a field of given 'name' in the 'fieldlist' of the given user-defined type and returns a pointer to the field entry. 
 * Returns NULL if the type does not have a field of the name.
 */
FieldList* field_list_get(char* name, TypeTable* type);

/**
 * Returns the amount of memory words required to store a variable of the given type.
 */
int get_size(TypeTable* type);

/**
 * Add field (name, type) to field list
 */
void field_list_add(FieldList* field_list, char* name, TypeTable* type);

/**
 * Create field with name and type 
 * Returns pointer to created field
 */
FieldList* field_create(char* name, TypeTable* type);

#endif