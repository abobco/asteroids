
// C program for linked list implementation of stack 

#include "stack.h"
  
struct StackNode* newNode(struct button_event data) 
{ 
    struct StackNode* stackNode = (struct StackNode*)malloc(sizeof(struct StackNode)); 
    stackNode->data = data;
    stackNode->next = NULL; 
    return stackNode; 
} 
  
int isEmpty(struct StackNode* root) 
{ 
    return !root; 
} 
  
void push(struct StackNode** root, int direction, int64_t finger_id) 
{ 
    struct button_event button = {direction, finger_id};
    struct StackNode* stackNode = newNode(button); 
    stackNode->next = *root; 
    *root = stackNode; 
} 
  
struct button_event pop(struct StackNode** root) 
{ 
    struct StackNode* temp = *root; 
    *root = (*root)->next; 
    struct button_event popped = temp->data; 
    free(temp); 
  
    return popped; 
} 
  
// struct button_event peek(struct StackNode* root) 
// { 
//     if (isEmpty(root)) 
//         return NULL; 
//     return root->data; 
// } 
  
