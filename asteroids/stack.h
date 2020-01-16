#include <limits.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h>

// touch and button event data
struct button_event{
	int button_id;
	int64_t finger_id;
};
// A structure to represent a stack 
struct StackNode { 
    struct button_event data;
    struct StackNode* next; 
}; 


struct StackNode* newNode(struct button_event data)  ;
int isEmpty(struct StackNode* root);
void push(struct StackNode** root, int direction, int64_t finger_id) ;
struct button_event pop(struct StackNode** root);
struct button_event peek(struct StackNode* root);