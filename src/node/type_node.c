#include "type_node.h"

VarType pointer_type(VarType type){
    switch(type){
        case TYPE_INT:
            return TYPE_INT_PTR;
        case TYPE_CHAR:
            return TYPE_CHAR_PTR;
    }
    printf("[WARNING] Pointer type undefined\n");
    return TYPE_NONE;
}

VarType variable_type(VarType ptr_type){
    switch(ptr_type){
        case TYPE_INT_PTR:
            return TYPE_INT;
        case TYPE_CHAR_PTR:
            return TYPE_CHAR;
    }
    printf("[WARNING] Pointer type undefined\n");
    return TYPE_NONE;
}

int is_pointer_type(VarType type){
    switch (type)
    {
    case TYPE_INT_PTR:
    case TYPE_CHAR_PTR:
        return 1;
    }
    return 0;
}