#ifndef _STACK_H
#define _STACK_H

typedef struct stack stack;

extern stack* Init(stack*);

extern void push(stack**,int element);

extern int pop(stack**);

extern int top(stack*);

extern void close(stack**);

extern char empty(stack*);
#endif