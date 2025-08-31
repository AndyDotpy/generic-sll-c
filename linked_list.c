#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"

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
	int (*compare)(const void * a, const void * b); // return < 0 if b > a ||| return 0 if a == b ||| return 1 if a > b. Pointers a and b point to two values/data
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

void set_free_ll(linked_list *list, void (*free_p)(void *)) {
	list->freev = free_p;
}

void set_print_ll(linked_list *list, void (*print_p)(void *)) {
	list->printv = print_p;
}

void set_deep_copy_ll(linked_list *list, void *(*deep_copy_p)(void * restrict destination, const void * restrict source, size_t size)) {
	list->deep_copyv = deep_copy_p;
}

void set_allocater_ll(linked_list *list, void *(allocate_p)(size_t size)) {
	list->allocate = allocate_p;
}

void set_compare_ll(linked_list *list, int (*compare_p)(const void *a, const void *b)) {
	list->compare = compare_p;
}

size_t get_size_ll(linked_list *list) {
	return list->size;
}

void empty_ll(linked_list* list) {
	node *curr = list->head;
	node *next;
	while (curr != NULL) {
		next = curr->next;
		list->freev(curr->value);
		free(curr);
		curr = next;
	}
	list->head = list->tail = NULL;
}

void free_linked_list(linked_list* list) {
	empty_ll(list);
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

void print_ll(linked_list *list) {
	if (list->printv == NULL) {
		printf("Attempted to call print_ll without giving the linked list a print function!\nYou can do so by set_print_ll\n");
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

void reverse_ll(linked_list *list) {
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

	if (func == NULL) {
		printf("Attempted to filter linked list when the filter function is NULL?\n");
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

void *iter_ll(linked_list *new_linked_list) {
	static node *curr = NULL;
	if (new_linked_list == NULL) {
		if (curr == NULL) {
			return NULL;
		}
		
		void *return_val = curr->value;
		curr = curr->next;
		return return_val;
	}

	curr = new_linked_list->head;
	return NULL;
}

linked_list *slice_linked_list(linked_list* list, size_t start, size_t end, void *(*allocator_p)(size_t)) {
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

linked_list *seperate_linked_list(linked_list *list, int(*func)(void *), void *(*allocator_p)(size_t)) { // func should return 1 if going in seperated list else 0
	linked_list *new_list = new_linked_list((allocator_p) ? allocator_p : list->allocate);
	new_list->printv = list->printv;
	new_list->freev = list->freev;
	new_list->deep_copyv = list->deep_copyv;
	new_list->compare = list->compare;
	new_list->size = 0;

	if (list == NULL || list->head == NULL) {
		return new_list;
	}

	node *new_curr = new_list->head;
	node *prev = NULL;
	node *curr = list->head;
	node *next = NULL;

	while (curr != NULL) {
		next = curr->next;

		if (func(curr->value)) {
			if (new_curr == NULL) {
				new_list->head = new_curr = curr;
			} else {
				new_curr->next = curr;
				new_curr = curr;
			}

			if (prev == NULL) {
				list->head = next;
			} else {
				prev->next = next;
			}

			new_list->size++;
			list->size--;
		} else {
			prev = curr;
		}
		curr = next;
	}
	
	list->tail = prev;
	list->tail->next = NULL;
	new_list->tail = new_curr;
	new_list->tail->next = NULL;

	return new_list;
}

void merge_sort_ll(linked_list *list) {
	if (list->head == NULL || list->head->next == NULL) {
		return;
	}

	if (list->compare == NULL) {
		printf("Called merge_sort_ll without giving the linked list a compare function?!\nSet it by set_compare_ll\n");
		return;
	}

	node** nodes = (node **) malloc (list->size * sizeof(node *));
	node** next_nodes = (node **) malloc (list->size * sizeof(node *));
	node** temp_nodes;
	size_t i;
	node* curr = list->head;

	for (i = 0; i < list->size; i++) {
		nodes[i] = curr;
		curr = curr->next;
	}

	curr = nodes[0];
	size_t endi;
	size_t endj;
	size_t j;
	int icontinue;
	int jcontinue;
	int isi;
	size_t subsize = 1;
	size_t subsize2;
	size_t k;

	while (subsize < list->size) {
		i = k = 0;
		j = endi = subsize;
		subsize2 = subsize << 1;
		endj = (list->size > subsize2) ? subsize2 : list->size;
		jcontinue = (j < endj);
		icontinue = (i < endi);
		while (i < list->size) {
			while (icontinue || jcontinue) {
				if (icontinue && jcontinue) {
					if (list->compare(nodes[i]->value, nodes[j]->value) >= 0) {
						isi = 1;
					} else {
						isi = 0;
					}
				} else {
					if (icontinue) {
						isi = 1;
					} else {
						isi = 0;
					}
				}
				if (isi) {
					next_nodes[k++] = nodes[i++];
					icontinue = (i < endi);
				} else {
					next_nodes[k++] = nodes[j++];
					jcontinue = (j < endj);
				}
			}
			i = endj;
			subsize2 = i + subsize;
			j = endi = (list->size > subsize2) ? subsize2 : list->size;
			subsize2 = j + subsize;
			endj = (list->size > subsize2) ? subsize2 : list->size;
			jcontinue = (j < endj);
			icontinue = (i < endi);
		}
		temp_nodes = nodes;
		nodes = next_nodes;
		next_nodes = temp_nodes;
		subsize <<= 1;
	}

	for (i = 1; i < list->size; i++) {
		nodes[i - 1]->next = nodes[i];
	}

	list->head = nodes[0];
	list->tail = nodes[list->size - 1];
	list->tail->next = NULL;
	free(nodes);
	free(next_nodes);
}

int is_sorted_ll(linked_list *list) {
	if (list->size <= 1) {
		return 1;
	}

	if (list->compare == NULL) {
		printf("Called is_sorted_ll without giving the linked list a compare function?!\nSet it by set_compare_ll\n");
		return 0;
	}

	for (node *curr = list->head; curr->next != NULL; curr = curr->next) {
		if (list->compare(curr->value, curr->next->value) == -1) {
			return 0;
		}
	}

	return 1;
}


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

void **get_as_array_ll(linked_list *list) { // Still need to free the linked list and the array, be careful if the data contains pointers to elsewhere the data is deep copied.
	if (list == NULL) {
		return NULL;
	}

	void **array = malloc (list->size * sizeof(void *));
	node *curr = list->head;
	size_t i = 0;

	while (curr != NULL) {
		array[i] = malloc (curr->val_size);
		list->deep_copyv(array[i], curr->value, curr->val_size);
		i++; curr = curr->next;
	}

	return array;
}

void **convert_to_array_ll(linked_list *list) { // This one creates the array while freeing the linked list
	if (list == NULL) {
		return NULL;
	}

	void **array = malloc (list->size * sizeof(void *));
	node *curr = list->head;
	node *prev = NULL;
	list->head = NULL;
	size_t i = 0;

	while (curr != NULL) {
		array[i] = malloc (curr->val_size);
		list->deep_copyv(array[i], curr->value, curr->val_size);
		list->freev(curr->value);
		prev = curr;
		i++; curr = curr->next;
		free(prev);
	}

	free(list);
	return array;
}

linked_list *array_to_linked_list(void **array, size_t size_of_array, size_t size_of_value, void *(*allocator_p)(size_t), void (*freev)(void *)) { // Assumes array is already stored on heap
	linked_list *list = new_linked_list(allocator_p);
	list->freev = (freev == NULL) ? free : freev;

	if (array == NULL) {
		return list;
	}

	list->head = (node *) list->allocate (sizeof(node));
	list->head->value = list->allocate (size_of_value);
	memcpy(list->head->value, array[0], size_of_value);
	list->head->val_size = size_of_value;
	list->freev(array[0]);
	list->tail = list->head;

	for (size_t i = 1; i < size_of_array; i++) {
		list->tail->next = (node *) list->allocate (sizeof(node));
		list->tail->next->value = list->allocate (size_of_value);
		memcpy(list->tail->next->value, array[i], size_of_value);
		list->tail->next->val_size = size_of_value;
		list->freev(array[i]);
		list->tail = list->tail->next;
	}
	list->tail->next = NULL;
	list->size = size_of_array;

	free(array);
	return list;
}

linked_list *array_as_linked_list(void **array, size_t size_of_array, size_t size_of_value, void *(*allocator_p)(size_t), void (*freev)(void*)) {// Assumes array is already stored on heap does not free array
	linked_list *list = new_linked_list(allocator_p);
	list->freev = (freev == NULL) ? free : freev;

	if (array == NULL) {
		return list;
	}

	list->head = (node *) list->allocate (sizeof(node));
	list->head->value = list->allocate (size_of_value);
	memcpy(list->head->value, array[0], size_of_value);
	list->head->val_size = size_of_value;
	list->tail = list->head;

	for (size_t i = 1; i < size_of_array; i++) {
		list->tail->next = (node *) list->allocate (sizeof(node));
		list->tail->next->value = list->allocate (size_of_value);
		memcpy(list->tail->next->value, array[i], size_of_value);
		list->tail->next->val_size = size_of_value;
		list->tail = list->tail->next;
	}
	list->tail->next = NULL;
	list->size = size_of_array;

	return list;
}


int internal_check_ll(linked_list *list, int fix) {// returns amount of tests failed 0 - 2
	printf("##### ##### ##### START OF INTERNAL CHECK ##### ##### #####\n");
	printf("_____ Function Addresses _____\n");
	printf("printv address:     %p\n", list->printv);
	printf("freev address:      %p\n", list->freev);
	printf("deep_copyv address: %p\n", list->deep_copyv);
	printf("allocate address:   %p\n", list->allocate);
	printf("compare address:    %p\n", list->compare);
	printf("list head address:  %p\n\n", list->head);

	char tests_passed = 0;
	size_t size = 0;
	node *curr = list->head;

	if (curr != NULL) {
		while (curr->next != NULL) {
			size++;
			curr = curr->next;
		}
		size++;
	}
	
	printf("_____ Tail and Size Tests _____\n");
	printf("Stored list size:  %zu\n", list->size);
	printf("Checked list size: %zu\n", size);
	if (list->size == size) {
		tests_passed++;
		printf("-> Size test passed!\n");
	} else {
		printf("-> Size test failed!\n");
	}
	
	printf("Stored tail address:  %p\n", list->tail);
	printf("Checked tail address: %p\n", curr);
	if (curr == list->tail) {
		tests_passed++;
		printf("-> Tail test passed!\n");
	} else {
		printf("-> Tail test failed!\n");
	}
	
	printf("\n_____ Overview _____\n");
	printf("%d/2 Tests passed\n\n", tests_passed);

	if (fix) {
		list->size = size;
		list->tail = curr;
		printf("Size and tail set to what was found during check.\n");
	} else if (tests_passed != 2) {
		printf("Failed tests not fixed.\nIf you want to fix them use fix_ll or this function with the fix parameter set to a non zero value.\n");
	}
	printf("##### ##### ##### -END- OF INTERNAL CHECK ##### ##### #####\n");

	return 2 - tests_passed;
}

void fix_ll(linked_list *list, int display_fix_message) {
	node *curr = list->head;
	size_t size = 0;

	if (curr != NULL) {
		while (curr->next != NULL) {
			size++;
			curr = curr->next;
		}
		size++;
	}

	if (display_fix_message) {
		if (size != list->size) {
			printf("Previous size: %zu\n", list->size);
			printf("Fixed size:    %zu\n", size);
		} if (curr != list->tail) {
			printf("Previous tail: %p\n", list->tail);
			printf("Fixed tail:    %p\n", curr);
		}
	}

	list->size = size;
	list->tail = curr;
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


