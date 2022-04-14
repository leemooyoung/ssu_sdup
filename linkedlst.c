#include <stdlib.h>
#include "linkedlist.h"

LNKLIST *linklist_init() {
	LNKLIST *head = (LNKLIST*)malloc(sizeof(LNKLIST));

	if(head == NULL)
		return NULL;

	head->prev = head;
	head->next = head;
	head->val = NULL;

	return head;
}

int lnklist_insert_after(LNKLIST *node, void *val) {
	if(node == NULL)
		return -1;

	LNKLIST *new_node = (LNKLIST*)malloc(sizeof(LNKLIST));

	if(new_node == NULL)
		return -2;

	node->next->prev = new_node;
	new_node->next = node->next;
	node->next = new_node;
	new_node->prev = node;
	new_node->val = val;

	return 0;
}

void *lnklist_delete(LNKLIST *node) {
	void *res;

	if(node == NULL)
		return NULL;

	node->prev->next = node->next;
	node->next->prev = node->prev;
	res = node->val;

	free(node);

	return res;
}

int lnklist_destroy(LNKLIST *head) {
	LNKLIST *node;
	LNKLIST *tmp;

	if(head == NULL)
		return -1;

	node = head->next;

	while(node != head) {
		tmp = node->next;
		
		if(node->val != NULL)
			free(node->val);
		free(node);

		node = tmp;
	}

	free(head);

	return 0;
}
