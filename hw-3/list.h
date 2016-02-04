#ifndef LIST_H
#define LIST_H

#include <stdint.h>

// The linked list is made of multiple list_t structures. Each list_t element
// has a `next` pointer that points to the next element of the list. The final
// element of the list's `next` pointer must be set to NULL.
typedef struct list_t {
	uint32_t index;
	struct list_t *next;
} list_t;

list_t* insert_sorted(list_t* head, list_t* new_element);
list_t* reverse(list_t* head);

#endif // LIST_H
