#ifndef HEADER_FILE
#define HEADER_FILE

typedef struct tnode{
    int val;
    char *op; //indicates the name of the operator for a non leaf node
    struct tnode *left, *right; //left and right branches
} tnode;


#define YYSTYPE tnode*

/*Make a leaf tnode and set the value of val field*/
tnode* makeLeafNode(int n);

/*Make a tnode with operator, left and right branches set*/
tnode* makeOperatorNode(char op, tnode *l, tnode *r);

/*Get prefix form*/
void prefix(tnode* node);

/*Get postfix form*/
void postfix(tnode* node);

#endif