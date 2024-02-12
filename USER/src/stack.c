#include"stack.h"
#include"stdlib.h"

struct stack
{
	int value;
	stack* first;
};

stack* Init(stack* sta)
{
	sta=malloc(sizeof(stack));
	sta->first=NULL;
	sta->value=NULL;
	return sta;
}

void push(stack** sta,int element)
{
	stack* temp=malloc(sizeof(stack));
	temp->first=*sta;
	temp->value=element;
	*sta=temp;
}

int pop(stack** sta)
{
	stack* temp;
	temp=(*sta)->first;
	free(*sta);
	*sta=temp;
}

void close(stack** sta)
{
	for(;(*sta)->first!=NULL;*sta=(*sta)->first)
		free(*sta);
	free(*sta);
	*sta=NULL;
}

char empty(stack* sta)
{
	return sta->first==NULL?1:0;
}

int top(stack* sta)
{
	return sta->value;
}