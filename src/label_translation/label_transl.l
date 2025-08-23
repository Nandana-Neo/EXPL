%{
    #include <stdio.h>    
    #include <string.h>
    #include "label_translation/label_transl.h"
    int label_transl_step = 0;
    FILE * input_file = NULL;
    FILE * output_file = NULL;
    int lines = 2040; // Considering first 8 lines 2056-16;
%}
label   _L[0-9]+
%%
{label}:\n         { 
                        if(label_transl_step==1){
                            //no printing
                        }
                        else{
                            int label_len = yyleng-2;   //remove last 2 chars
                            char * label = strndup(yytext,label_len);
                            create_label_node(label,lines);
                        }
                    }
\n                  { 
                        if(label_transl_step==1){
                            fprintf(output_file,"\n");
                        }
                        else{
                            lines+=2;
                        }
                    }
(.)                   {
                        if(label_transl_step==1)
                            fprintf(output_file,"%c",*yytext);
                    }
{label}             {   
                        if(label_transl_step==1){
                            int address = get_address(yytext);
                            fprintf(output_file,"%d",address);
                        }
                    }

%%
int yywrap(){
    if(label_transl_step == 0){
        label_transl_step = 1;
        rewind(yyin);
        return 0;
    }
    return 1;
}
int main(int argc, char* argv[]){
    input_file = fopen(argv[2], "r");   // after -i
    output_file = fopen(argv[4],"w");   // after -o
    if(output_file == NULL || input_file == NULL){
        printf("Error opening file\n");
    }
    yyin = input_file;
    yylex();
    return 0;
}