#ifndef STACK_H
#define STACK_H
struct node {
   int val;
   struct node *prev;
   struct node *next;
};
struct stack {
   struct node *top;
};
void push(struct stack *st, int val);
int pop(struct stack *st);

#endif
