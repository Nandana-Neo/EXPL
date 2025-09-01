%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "node/ast_node.h"
    #include "y.tab.h"
    int yywrap();
%}

%%
[0-9]+  {
    int val = atoi(yytext);
    yylval.ast_node = make_leaf_node(val, TYPE_INT, NULL);
    return NUM;
}

[a-z]   {
    char* variable = strdup(yytext);    // ** Value is assumed to be int
    yylval.id_name = variable;
    return ID;
}

[()+*/=<>!,;-] {
    return *yytext;
}

[ \n]       { }

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
%%
int yywrap(){
    return 1;
}