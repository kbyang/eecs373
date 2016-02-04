#include <assert.h>
#include <stdlib.h>

#include "list.h"

// Takes a valid, sorted list starting at `head` and adds the element
// `new_element` to the list. The list is sorted based on the value of the
// `index` member in ascending order. Returns a pointer to the head of the list
// after the insertion of the new element.
list_t* insert_sorted(list_t* head, list_t* new_element) {
	assert(head != NULL);
	assert(new_element != NULL);

	list_t* prev = NULL;
	list_t* pos = head;
	new_element->next = NULL;

	while (pos != NULL) {
		if (new_element->index <= pos->index) {
			new_element->next = pos;
			if (prev == NULL) {
				head = new_element;
			} else {
				prev->next = new_element;
			}
			break;
		}

		prev = pos;
		pos = pos->next;
	}

	if (new_element->next == NULL) {
		prev->next = new_element;
	}

	return head;
}

// Reverses the order of the list starting at `head` and returns a pointer to
// the resulting list. You do not need to preserve the original list.
list_t* reverse(list_t* head) {
	assert(head != NULL);

	list_t* prev = NULL;
	list_t* pos = head;

	while (pos != NULL) {
		list_t* go = pos->next;

		pos->next = prev;

		prev = pos;
		pos = go;
	}
	
	head = prev;

	return head;
}

