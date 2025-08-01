%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    char buffer[100];
    int depth = 0;
    char c;
%}

%token IF
%token THEN
%token ELSE
%token ID
%token NUM
%token RELOP
%nonassoc RELOP
%left '+'
%left '*'

%%

start   :   expr '\n' '\n'  {printf("Complete\n"); exit(0);}
        ;

expr    :   '(' expr ')'    {}
        |   IF comp THEN expr ELSE expr '\n' { depth--; printf("Level:%d\n",$1);}
        |   IF comp THEN expr '\n'  {depth--; printf("Level:%d\n",$1);}
        |   var '+' var     {}
        |   var '*' var     {}
        |   expr expr       {}
        ;

comp    :   var RELOP var   {}
        ;

var     :   NUM     {}
        |   ID      {}
        ;
%%

yyerror() {
	printf("Error\n");
}

yylex() {
    if(c==' '){
        c = getchar();
        return yylex();
    }
    else if(c>='0' && c<='9'){
        while(c>='0' && c<='9'){
            c = getchar();
        }
        return NUM;
    }
    else if((c>='a' && c<='z') || (c>='A' && c<='Z')){
        int pos = 0;
        buffer[0] =  c;
        pos++;
        c=getchar();
        while(c!='\n' && c!=' ' && c!='\0'){
            buffer[pos++] = c;
            c=getchar();
        }
        buffer[pos] = '\0';
        if(strcmp(buffer,"if") == 0){
            yylval = depth;
            depth++;
            return IF;
        }
        else if(strcmp(buffer,"then") == 0){
            return THEN;
        }
        else if(strcmp(buffer,"else") == 0){
            return ELSE;
        }
        return ID;
    }
    else if(c=='<' || c=='>'){
        c = getchar();
        if(c=='='){
            c = getchar();
        }
        return RELOP;
    }
    else{
        char l = c;
        c = getchar();
        return l;
    }
}

int main() {
    c = getchar();
    yyparse();
    return 1;
}
