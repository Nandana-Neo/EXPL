%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "node/ast_node.h"
    #include "node/type_node.h"
    #include "y.tab.h"
    int yywrap();
%}

%%
[0-9]+  {
    node_val val;
    val.int_val = atoi(yytext);
    Type* type = create_type(type_table_get("int"),0);
    yylval.ast_node = make_leaf_node(val, type, NULL, NULL, NULL);
    free(type);
    return NUM_VAL;
}


\"[^"\n]*\"       { //safe way to match a string (not include " and \n)
    node_val val;
    val.str_val = strdup(yytext);
    Type* type = create_type(type_table_get("str"),0);
    yylval.ast_node = make_leaf_node(val, type, NULL, NULL, NULL);
    free(type);
    return STR_VAL;
}

"begin"     {   return P_BEGIN;   }
"end"       {   return P_END;     }
"read"      {   return READ;    }
"write"     {   return WRITE;   }
"if"        {   return IF;      }
"then"      {   return THEN;    }
"else"      {   return ELSE;    }
"endif"     {   return ENDIF;   }
"while"     {   return WHILE;   }
"endwhile"  {   return ENDWHILE;    }
"do"        {   return DO;      }
"break"     {   return BREAK;   }
"continue"  {   return CONTINUE;    }
"repeat"    {   return REPEAT;  }
"until"     {   return UNTIL;   }
"int"       {   return INT;     }
"str"       {   return STR;     }
"decl"      {   return DECL;    }
"enddecl"   {   return ENDDECL; }
"int main"  {   return MAIN_DEF;}
"return"    {   return RETURN_STMT; }
"AND"       {   return AND; }
"OR"        {   return OR;  }
"NOT"       {   return NOT; }
"type"      {   return TYPE;    }
"endtype"   {   return ENDTYPE; }
"initialize"    {   return INITIALIZE;  }
"alloc"     {   return ALLOC;   }
"free"      {   return FREE;    }

[a-zA-Z]+[0-9a-zA-Z_]*   {
    char* variable = strdup(yytext);
    yylval.id_name = variable;
    return ID;
}

[()+*/.%={}<>!,;\[\]&-] {
    return *yytext;
}

[ \n]       { }

%%
int yywrap(){
    return 1;
}