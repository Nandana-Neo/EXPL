#include "code_gen.h"

static int regNum = 0;

int getReg(){
    if(regNum==19){
        fprintf(stderr, "\nOut of free registers\n");  //error
        exit(1);
    }
    return regNum++;
}

int freeReg(){
    if(regNum == 0){
        fprintf(stderr,"\nNo registers to be freed\n"); //error
        exit(1);
    }
    regNum--;
    return 0;
}