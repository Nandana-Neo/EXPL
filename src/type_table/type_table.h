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
    struct FieldList *fields;   //pointer to the head of fields list -- if struct
    struct TypeTable *next;     // pointer to the next type table entry
} TypeTable;

// wrapper around type table entry with pointer type info
typedef struct Type{
  TypeTable* type_table;
  int ptr;            // 1 if pointer, else 0
}Type;

extern TypeTable* type_table;

/**
 * Function: type_table_init
 * ---------------------------
 * Function to initialise the type table entries with primitive types (int,str) and special entries_(boolean,null,void).
 */
void type_table_init();

/**
 * Add primitive data type into type table
 * Returns NULL if failed
 */
TypeTable* type_table_add_primitive(VarType type);


/**
 * Search through the type table and return pointer to type table entry of type 'name'. 
 * Returns NULL if entry is not found.
 */ 
TypeTable* type_table_get(char *name);

/**
 * Creates temporary type table data structure node without connection to main type table
 * Used to check later on while parsing
 */ 
TypeTable* create_temp_type_table(char* name);

/**
 * Creates a type table entry for the (user defined) type of 'name' with given 'fields' and returns the pointer to the type table entry. 
 * The field list must specify the field index, type and name of each field. 
 * 
 * Returns NULL upon failure. This routine is invoked when the compiler encounters a type definition in the source program.
 * - size is automatically computed according to field list length
 */ 
TypeTable* type_table_add(char* name, int size, FieldList* fields);

/**
 * Update the field type and replace all the temp types
 * If any type is not in the table, throw error
 */
void update_field_types(TypeTable* type);

/**
 * Updates the size of the type based on the field list
 */
void update_type_size(TypeTable* type);

/**
 * Returns the amount of memory words required to store a variable of the given type.
 */
int get_type_size(TypeTable* type);

/**
 * Searches for a field of given 'name' in the 'fieldlist' of the given user-defined type and returns a pointer to the field entry. 
 * Returns NULL if the type does not have a field of the name.
 */
FieldList* field_list_get(char* name, TypeTable* type);


/**
 * Create field with name and type 
 * Returns pointer to created field
 */
FieldList* field_create(char* name, TypeTable* type);

/**
 * Add field (name, type) to field list
 */
FieldList* field_list_add(FieldList* field_list, char* name, TypeTable* type);

/**
 * Joins field list f2 to the end of f1
 */
FieldList* field_list_join(FieldList* f1, FieldList* f2);

Type* create_type(TypeTable* type_table, int ptr);
/**
 * Compare the types of the nodes
 * Returns 1 if same 
 * else 0
 */
int compare_type(Type* t1, Type* t2);


/**
 * Compare the typetable pointers of the nodes
 * Returns 1 if pointing to same type
 * else 0
 */
int compare_type_table(TypeTable* t1, TypeTable* t2);

/**
 * Prints the type table after completion of creation of the table
 */
void print_type_table();

/************************************************************************* */
/*                         Primitive Comparison Fns                        */
/************************************************************************* */
int is_int(Type* type);
int is_int_ptr(Type* type);

int is_str(Type* type);
int is_str_ptr(Type* type);

int is_bool(Type* type);

int is_null(Type* type);

int is_void(Type* type);
#endif