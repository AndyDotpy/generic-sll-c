#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node {
	void *value;
	size_t val_size;
	struct node *next;
} node;

typedef struct linked_list {
	size_t size;
	void (*printv)(void*);
	void (*freev)(void*);
	void *(*deep_copyv)(void * restrict destination, const void * restrict source, size_t size);
	void *(*allocate)(size_t size);
	int (*compare)(void * a, void * b); // return < 0 if b > a ||| return 0 if a == b ||| return 1 if a > b
	node *head;
	node *tail;
} linked_list;

linked_list *new_linked_list(void *(*allocator_p)(size_t)) {
	void *(*allocator)(size_t) = (allocator_p == NULL) ? malloc : allocator_p;
	linked_list *new_list = (linked_list*) allocator (sizeof(linked_list));
	new_list->size = 0;
	new_list->printv = NULL;
	new_list->freev = free;
	new_list->deep_copyv = memcpy;
	new_list->allocate = allocator;
	new_list->compare = NULL;
	new_list->head = NULL;
	new_list->tail = NULL;
	return new_list;
}

void empty_linked_list(linked_list* list) {
	node *curr = list->head;
	node *next;
	while (curr != NULL) {
		next = curr->next;
		list->freev(curr->value);
		free(curr);
		curr = next;
	}
}

void free_linked_list(linked_list* list) {
	empty_linked_list(list);
	free(list);
}

int is_empty_ll(linked_list *list) {
	if (list->head == NULL) {
		return 1;
	} return 0;
}

void prepend_ll(linked_list *list, void *data, size_t data_size) {
	node *new_head = (node*) list->allocate (sizeof(node));

	if (is_empty_ll(list)) {
		list->tail = new_head;
	}

	new_head->value = list->allocate (data_size);
	list->deep_copyv(new_head->value, data, data_size);
	new_head->val_size = data_size;
	new_head->next = list->head;
	list->head = new_head;
	list->size++;
}

size_t append_ll(linked_list *list, void *data, size_t data_size) {
	if (is_empty_ll(list)) {
		prepend_ll(list, data, data_size);
		return 0;
	}

	list->tail->next = (node*) list->allocate (sizeof(node));
	list->tail->next->val_size = data_size;
	list->tail->next->value = list->allocate (data_size);
	list->deep_copyv(list->tail->next->value, data, data_size);
	list->tail = list->tail->next;
	list->tail->next = NULL;
	return list->size++;
}

int insert_ll(linked_list *list, void *data, size_t data_size, size_t index) {
	if (index >= list->size) {
		return 0;
	}

	if (is_empty_ll(list)) {
		if (index == 0) {
			prepend_ll(list, data, data_size);
			return 1;
		}
		return 0;
	}

	if (index == 0) {
		prepend_ll(list, data, data_size);
		return 1;
	}

	if (index == (list->size - 1)) {
		append_ll(list, data, data_size);
		return 1;
	}

	node *curr = list->head;
	size_t curr_index = 0;
	index--;
	while (curr != NULL && curr_index < index) {
		curr = curr->next;
		curr_index++;
	}

	if (curr_index != index) {
		return 0;
	}

	node *new_node = (node*) list->allocate (sizeof(node));
	new_node->value = list->allocate (data_size);
	list->deep_copyv(new_node->value, data, data_size);
	new_node->val_size = data_size;
	new_node->next = curr->next;
	curr->next = new_node;
	list->size++;
	return 1;
}

int delete_ll(linked_list *list, size_t index) {
	if (list->head == NULL || index >= list->size) {
		return 0;
	}

	node *prev;
	node *curr = list->head;

	if (index == 0) {
		list->head = curr->next;
		if (curr == list->tail) {
			list->tail = list->head;
		}
		list->freev(curr->value);
		free(curr);
		list->size--;
		return 1;
	}

	size_t curr_index = 0;
	while (curr != NULL && curr_index < index) {
		prev = curr;
		curr = curr->next;
		curr_index++;
	}

	prev->next = curr->next;
	if (curr == list->tail) {
		list->tail = prev;
	}
	list->freev(curr->value);
	free(curr);
	list->size--;
	return 1;
}

void *extract_head_ll(linked_list *list) {
	if (list->head == NULL) {
		return NULL;
	}

	void *return_val = list->head->value;

	node *old_head = list->head;
	list->head = list->head->next;
	if (list->size == 1) {
		list->tail = list->head;
	}
	free(old_head);
	list->size--;
	return return_val;
}

void *extract_ll(linked_list *list, size_t index) {
	if (index == 0) {
		extract_head_ll(list);
	} if (list->head == NULL || index >= list->size) {
		return NULL;
	}

	size_t curr_index = 1;
	node *prev = list->head;
	node *curr = list->head->next;

	while (curr_index < index && curr != NULL) {
		prev = curr;
		curr = curr->next;
		curr_index++;
	} if (curr_index != index || curr == NULL) {
		return NULL;
	} if (curr == list->tail) {
		list->tail = prev;
	}

	prev->next = curr->next;
	void *return_val = curr->value;
	free(curr);
	list->size--;
	return return_val;
}

void print_linked_list(linked_list *list) {
	if (list->printv == NULL) {
		printf("printv function is required\n");
		return;
	}

	node *curr = list->head;
	size_t index = 0;
	printf("\n----- ----- Linked List Start ----- -----\n");

	while (curr != NULL) {
		printf("Index: %zu ", index);
		list->printv(curr->value);
		curr = curr->next;
		index++;
	}

	printf("\n----- ----- Linked List |End| ----- -----\n");
}

linked_list *clone_linked_list(linked_list *list, void *(*allocator_p)(size_t)) {
	linked_list *cloned_list = new_linked_list((allocator_p) ? allocator_p :list->allocate);
	cloned_list->size = list->size;
	cloned_list->printv = list->printv;
	cloned_list->freev = list->freev;
	cloned_list->deep_copyv = list->deep_copyv;
	cloned_list->compare = list->compare;

	if (is_empty_ll(list)) {
		return cloned_list;
	}

	cloned_list->head = (node*) cloned_list->allocate (sizeof(node));
	cloned_list->head->value = cloned_list->allocate (list->head->val_size);
	list->deep_copyv(cloned_list->head->value, list->head->value, list->head->val_size);
	cloned_list->head->val_size = list->head->val_size;

	node *clone_curr = cloned_list->head;
	node *curr = list->head;

	if (curr->next == NULL) {
		cloned_list->head->next = NULL;
		cloned_list->tail = cloned_list->head;
		return cloned_list;
	}

	curr = curr->next;

	while (curr != NULL) {
		clone_curr->next = (node*) cloned_list->allocate (sizeof(node));
		clone_curr = clone_curr->next;
		clone_curr->value = cloned_list->allocate (curr->val_size);
		list->deep_copyv(clone_curr->value, curr->value, curr->val_size);
		clone_curr->val_size = curr->val_size;
		curr = curr->next;
	}

	cloned_list->tail = clone_curr;
	cloned_list->tail->next = NULL;
	return cloned_list;
}

void reverse_linked_list(linked_list *list) {
	if (is_empty_ll(list) || list->head->next == NULL) {
		return;
	}

	node *prev = list->head;
	node *curr = list->head->next;
	node *next = list->head->next->next;
	list->head->next = NULL;

	while (next != NULL) {
		curr->next = prev;
		prev = curr;
		curr = next;
		next = next->next;
	}

	curr->next = prev;
	node *temp = list->head;
	list->head = list->tail;
	list->tail = temp;
}

void *get_data_ll(linked_list *list, size_t index) {
	if (index >= list->size || is_empty_ll(list)) {
		return NULL;
	}

	if (index == list->size - 1) {
		return list->tail->value;
	}
	
	node *curr = list->head;
	size_t curr_index = 0;
	while (curr != NULL && curr_index < index) {
		curr = curr->next;
		curr_index++;
	}

	if (curr_index == index) {
		return curr->value;
	} return NULL;
}

void map_ll(linked_list *list, void (*func)(void *)) {
	node *curr = list->head;
	while (curr != NULL) {
		func(curr->value);
		curr = curr->next;
	}
}

void filter_ll(linked_list *list, int (*func)(void *)) { // func should return 1 if the value should be removed else 0
	if (is_empty_ll(list)) {
		return;
	}

	node *prev = list->head;
	node *curr = list->head->next;

	while (func(prev->value)) {
		list->freev(prev->value);
		free(prev);
		list->size--;
		if (curr != NULL) {
			prev = curr;
			curr = curr->next;
		} else {
			list->head = NULL;
			list->tail = NULL;
			return;
		}
	}

	list->head = prev;

	while (curr != NULL) {
		if (func(curr->value)) {
			prev->next = curr->next;
			list->freev(curr->value);
			free(curr);
			list->size--;
		} else {
			prev = curr;
		}
		curr = prev->next;
	}

	list->tail = prev;
	list->tail->next = NULL;
}

void *each_ll(node *new_curr) {
	static node *curr = NULL;
	if (new_curr == NULL) {
		if (curr == NULL) {
			return NULL;
		}
		
		void *return_val = curr->value;
		curr = curr->next;
		return return_val;
	}

	curr = new_curr;
	return NULL;
}

linked_list *slice_ll(linked_list* list, size_t start, size_t end, void *(*allocator_p)(size_t)) {
	if (end < start) {
		printf("Start index cannot be greater than end index...\n");
		return NULL;
	} else if (end >= list->size || is_empty_ll(list)) {
		return NULL; // No message here
	}

	linked_list *new_list = new_linked_list((allocator_p) ? allocator_p : list->allocate);
	new_list->printv = list->printv;
	new_list->freev = list->freev;
	new_list->deep_copyv = list->deep_copyv;
	new_list->compare = list->compare;
	new_list->size = end - start + 1;

	node *curr = list->head;
	size_t cur_index = 0;
	while (cur_index < start) {
		curr = curr->next;
		cur_index++;
	}

	new_list->head = (node*) new_list->allocate (sizeof(node));
	new_list->head->value = new_list->allocate (curr->val_size);
	new_list->head->val_size = curr->val_size;
	list->deep_copyv(new_list->head->value, curr->value, curr->val_size);
	node *new_curr = new_list->head;
	
	while (cur_index < end) {
		curr = curr->next;
		new_curr->next = (node*) new_list->allocate (sizeof(node));
		new_curr = new_curr->next;
		new_curr->value = new_list->allocate (curr->val_size);
		list->deep_copyv(new_curr->value, curr->value, curr->val_size);
		new_curr->val_size = curr->val_size;
		cur_index++;
	}

	new_list->tail = new_curr;
	new_list->tail->next = NULL;

	return new_list;
}

linked_list *seperate_ll(linked_list *list, int(*func)(void *), void *(*allocator_p)(size_t)) { // func should return 1 if going in seperated list else 0
	linked_list *new_list = new_linked_list((allocator_p) ? allocator_p : list->allocate);
	new_list->printv = list->printv;
	new_list->freev = list->freev;
	new_list->deep_copyv = list->deep_copyv;
	new_list->compare = list->compare;
	new_list->size = 0;

	node *new_curr = new_list->head;

	while (func(list->head->value)) {
		new_curr = list->head;
		new_curr = new_curr->next;
		list->head = list->head->next;
	} // Yea bruh this is annoying

	return new_list;
}

// TODO make a sorting method that works efficiently

// TODO make a get index based off of val method
size_t get_index_ll(linked_list *list, void *value, size_t occurrence) { // returns 0 <= x < size if successfull else returns size
	if (list == NULL || value == NULL || !occurrence) {
		return list->size;
	}

	size_t curr_index = 0;
	node *curr = list->head;

	while (curr) {
		if (!list->compare(curr->value, value)) {
			occurrence--;
			if (!occurrence) {
				return curr_index;
			}
		}
		curr = curr->next;
		curr_index++;
	}

	return list->size;
}

int get_int_val_ll(linked_list *list, size_t index) {
	return *(int*) get_data_ll(list, index);
}

unsigned int get_uint_val_ll(linked_list *list, size_t index) {
	return *(unsigned int*) get_data_ll(list, index);
}

long get_long_val_ll(linked_list *list, size_t index) {
	return *(long*) get_data_ll(list, index);
}

unsigned long get_ulong_val_ll(linked_list *list, size_t index) {
	return *(unsigned long*) get_data_ll(list, index);
}

short get_short_val_ll(linked_list *list, size_t index) {
	return *(short*) get_data_ll(list, index);
}

unsigned short get_ushort_val_ll(linked_list *list, size_t index) {
	return *(unsigned short*) get_data_ll(list, index);
}

float get_float_val_ll(linked_list *list, size_t index) {
	return *(float*) get_data_ll(list, index);
}

double get_double_val_ll(linked_list *list, size_t index) {
	return *(double*) get_data_ll(list, index);
}

char get_char_val_ll(linked_list *list, size_t index) {
	return *(char*) get_data_ll(list, index);
}

unsigned char get_uchar_val_ll(linked_list *list, size_t index) {
	return *(unsigned char*) get_data_ll(list, index);
}

char *get_str_val_ll(linked_list *list, size_t index) {
	return *(char**) get_data_ll(list, index);
}


void print_as_int(void *value) {
	printf("Integer: %d\n", *(int*)value);
}

int remove_all_ll(void *placeholder) {
	return 1;
}

int remove_odd_values(void *data) {
	return (*(int *)data) % 2;
}


int main() {
	int values[] = {1, 2, 3, 4, 5, 6, 7, 8};
	linked_list *my_list = new_linked_list(NULL);
	my_list->printv = print_as_int;
	int i = 0;
	printf("----- ----- Adding Test ----- -----\n");
	for (i = 0; i < 8; i++) {
		printf("Int added at index: %zu\n", append_ll(my_list, &values[i], sizeof(int)));
	}
	print_linked_list(my_list);
	printf("\nCurrent Size: %zu\n", my_list->size);
	printf("----- ----- Removing Test ----- -----\n");
	for (i = 0; i < 11; i += 2) {
		printf("Attempt to remove data at index: %d, result: %d\n", i, delete_ll(my_list, i));
	}
	print_linked_list(my_list);
	printf("\nCurrent Size: %zu\n", my_list->size);
	printf("----- Two threes added at front -----\n");
	prepend_ll(my_list, &values[2], sizeof(int));
	prepend_ll(my_list, &values[2], sizeof(int));
	print_linked_list(my_list);
	printf("\nCurrent Size: %zu\n", my_list->size);
	insert_ll(my_list, &values[6], sizeof(int), 5);
	insert_ll(my_list, &values[6], sizeof(int), 7);
	printf("----- Inserted 7 at index 5 and 7 -----\n");
	print_linked_list(my_list);
	printf("\nCurrent Size: %zu\n", my_list->size);
	linked_list *clone_list = clone_linked_list(my_list, NULL);
	delete_ll(clone_list, 0);
	printf("----- ----- Cloned List (first element removed) ----- -----\n");
	print_linked_list(clone_list);
	printf("\nCurrent Clone Size: %zu\n", clone_list->size);
	printf("----- ----- Reverse Linked List Test ----- -----\n");
	reverse_linked_list(clone_list);
	print_linked_list(clone_list);
	printf("\n----- ----- Getting Values Test (from cloned list) ----- -----\n");
	printf("Index 0: %d\n", get_int_val_ll(clone_list, 0));
	printf("Index 5: %d\n", get_int_val_ll(clone_list, 5));
	printf("\n----- ----- Extraction Test ----- -----\n");
	printf("----- my_list before extractions -----\n");
	print_linked_list(my_list);
	void *extract1 = extract_ll(my_list, 3);
	void *extract2 = extract_head_ll(my_list);
	void *extract3 = extract_ll(my_list, 6);
	printf("Extracted from index 3: %d\n", *(int*)extract1);
	printf("Extracted from head: %d\n", *(int*)extract2);
	printf("Extracted from tail (index 6) %d\n", *(int*)extract3);
	printf("----- my list after extractions -----\n");
	print_linked_list(my_list);
	printf("\n----- ----- Filtering tests on cloned list ----- -----\n");
	printf("----- Current Cloned List -----\n");
	print_linked_list(clone_list);
	printf("----- Cloned List after odd numbers were filtered -----\n");
	filter_ll(clone_list, remove_odd_values);
	print_linked_list(clone_list);
	printf("----- Cloned List after all values were filtered -----\n");
	filter_ll(clone_list, remove_all_ll);
	print_linked_list(clone_list);
	printf("----- ----- Re populating cloned list ----- -----\n");
	for (i = 0; i < 8; i++) {
		printf("Int added at index: %zu\n", append_ll(clone_list, &values[i], sizeof(int)));
	}
	print_linked_list(clone_list);
	printf("----- Slicing cloned list from 1 - 4 and printing it-----");
	linked_list *sliced_list = slice_ll(clone_list, 1, 4, NULL);
	linked_list *head_slice = slice_ll(clone_list, 0, 0, NULL);
	linked_list *tail_slice = slice_ll(clone_list, clone_list->size - 1, clone_list->size - 1, NULL);
	print_linked_list(sliced_list);
	printf("----- Sliced head printed below -----\n");
	print_linked_list(head_slice);
	printf("----- Sliced tail printed below -----\n");
	print_linked_list(tail_slice);
	printf("----- ----- Cloned list after the slices ----- -----\n");
	print_linked_list(clone_list);
	printf("----- Testing each_ll by adding up everything in the sliced_list -----\n");
	each_ll(sliced_list->head);
	// i defined above
	int total = 0;
	void *ret;
	while (i = ((ret = each_ll(NULL)) ? *(int*)ret: 0)) {
		total += i;
	}
	printf("Total of sliced_list = %d\n", total);
	
	free(extract1);
	free(extract2);
	free(extract3);
	free_linked_list(head_slice);
	free_linked_list(tail_slice);
	free_linked_list(sliced_list);
	free_linked_list(my_list);
	free_linked_list(clone_list);
	return 0;
}
