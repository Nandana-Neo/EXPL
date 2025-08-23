#include "label_transl.h"

label_node* head = NULL;

label_node* create_label_node(char* label, int address){
    label_node* node = (label_node*)malloc(sizeof(label_node));
    node->label = label;
    node->address = address;
    node->next = head;
    head = node;
    return node;
}

int get_address(char* label){
    label_node* temp = head;
    while(temp!=NULL){
        if(strcmp(temp->label,label)==0){
            return temp->address;
        }
        temp = temp->next;
    }
    return -1;  //error
}

void print_node(){

    label_node* temp = head;
    while(temp!=NULL){
        printf("%s - %d\n",temp->label,temp->address);
        temp = temp->next;
    }
    return;
}