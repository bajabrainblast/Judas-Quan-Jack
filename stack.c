#include "stack.h"
#include <stdlib.h>

struct stack st;

void push(struct stack *st, int val) {
   struct node *new_node = (struct node *) malloc(sizeof(struct node));
   new_node->val = val;
   if (st->top == NULL) {
      st->top = new_node;
      new_node->prev = NULL;
      new_node->next = NULL;
   }
   else {
      new_node->prev = st->top;
      st->top->next = new_node;
      new_node->next = NULL;
      st->top = new_node;
   }
}

int pop(struct stack *st) {
   if (st->top == NULL) {
      return (-1);
   }
   else {
      struct node *temp = st->top;
      int id = temp->val;
      st->top = temp->prev;
      if (st->top != NULL)
         st->top->next = NULL;
      free(temp);
      return id;
   }
}
