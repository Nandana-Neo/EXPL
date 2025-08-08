%{
    #include <stdio.h>
    #include <stdlib.h>
    char c;
    
%}

%token LETTER
%token DIGIT

%%
start : var '\n'    {printf("Valid\n"); exit(0);}
        ;
var     :   LETTER nvar {}  
        ;
nvar    :   LETTER      {}
        |   DIGIT       {}
        |   nvar nvar   {}
        ;
%%

yyerror() {
	printf("Error\n");
}

yylex() {
    c = getchar();
    if((c>='a' && c<='z')||(c>='A' && c<='Z')){
        return LETTER;
    }
    else if(c>='0' && c<='9')
        return DIGIT;
    else
        return c;
}

int main() {
    yyparse();
    return 1;
}
