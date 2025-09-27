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
    yylval.ast_node = make_leaf_node(val, TYPE_INT, NULL, NULL, NULL);
    return NUM_VAL;
}


\"[^"\n]*\"       { //safe way to match a string (not include " and \n)
    node_val val;
    val.str_val = strdup(yytext);
    yylval.ast_node = make_leaf_node(val, TYPE_STR, NULL, NULL, NULL);
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


[a-zA-Z]+[0-9a-zA-Z_]*   {
    char* variable = strdup(yytext);
    yylval.id_name = variable;
    return ID;
}

[()+*/%={}<>!,;\[\]&-] {
    return *yytext;
}

[ \n]       { }

%%
int yywrap(){
    return 1;
}