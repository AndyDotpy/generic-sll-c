#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stddef.h>

/* Opaque type as linked_list, so user cannot accidentally break the linked list for example
	list->head = list->head->next; without freeing the original list->head
*/
typedef struct linked_list linked_list;

/*

IMPORTANT NOTES:

All functions that have linked_list in the name instead of ll either create a new linked_list that has to be freed or it is freeing the linked list.

Remember to free all linked lists with free_linked_list and remember to free extracts

When a linked list is created it has no print or compare function but the deep copy function
defaults to memcpy the free function defaults to free and that allocate function defaults to malloc

malloc is not checked for failure

*/

// Constructor for linked list, you can send in an allocator function or you can use NULL, defaults to malloc
linked_list *new_linked_list(void *(*allocator_p)(size_t));

// Sets the function that frees the data the link lists holds, must be set if the data being held
// contains pointers to other memory that needs to be freed
void set_free_ll(linked_list *list, void (*free_p)(void *));

// Sets the function that displays your data
void set_print_ll(linked_list *list, void (*print_p)(void *));

// Sets the function that deep copies your data, must be set if the data being held contains
// pointers to other memory that needs to be copied
void set_deep_copy_ll(linked_list *list, void *(*deep_copy_p)(void * restrict destination, const void * restrict source, size_t size));

// Sets the allocater function for data the linked list will store
void set_allocater_ll(linked_list *list, void *(allocate_p)(size_t size));

// Sets the compare function that will be used to compare data stored
void set_compare_ll(linked_list *list, int (*compare_p)(const void *a, const void *b));

// Returns a size_t representing how many elements are in the linked list
size_t get_size_ll(linked_list *list);

// Frees everything the linked list has except for the linked list struct itself
void empty_ll(linked_list* list);

// Frees the linked list and everything it has in it
void free_linked_list(linked_list* list);

// If the head is NULL it returns 1 else 0
int is_empty_ll(linked_list *list);

// Adds data to the front of the linked list O(1)
void prepend_ll(linked_list *list, void *data, size_t data_size);

// Adds data to the end of the linked list O(1) there is a tail pointer
size_t append_ll(linked_list *list, void *data, size_t data_size);

// Adds data to the given index of the linked list O(n)
int insert_ll(linked_list *list, void *data, size_t data_size, size_t index);

// Deletes whats at given index
int delete_ll(linked_list *list, size_t index);

// Returns the void * to value at the head, moves head node to head->next and deletes old head node
// Must be freed
void *extract_head_ll(linked_list *list);

// Returns the void * to value at given index and deletes the node
// Must be freed
void *extract_ll(linked_list *list, size_t index);

// Displays all contents of the linked list make sure print function is set
void print_ll(linked_list *list);

// Deep copies the linked list and returns it, optional: send in an allocator method
// You can put NULL it will default to the original lists allocator
linked_list *clone_linked_list(linked_list *list, void *(*allocator_p)(size_t));

// (In place) Reverses the nodes of the linked list
void reverse_ll(linked_list *list);

// Returns the void * to the data at given index, unlike extract it does not remove the node and it still in the linked list
void *get_data_ll(linked_list *list, size_t index);

// Applies a given function to every element in the linked list
void map_ll(linked_list *list, void (*func)(void *));

// Deletes the nodes and values within the nodes if func returns 1 when the point to the value is passed in, goes through every element in the linked list
void filter_ll(linked_list *list, int (*func)(void *));

// Iterator, initially pass in linked_list pointer then to start getting values pass in NULL
// First time NULL is sent in you get a void * to the first value, second time NULL is passed in
// you get a void * to the second value. All the way until you iterated over the entire linked list
// where NULL would be returned at the end.
void *iter_ll(linked_list *new_linked_list);

// Deep copies the start < end indexed nodes into a new linked list
// allocator_p can be used to send in your own allocator else it will use the allocator from the given linked list
linked_list *slice_linked_list(linked_list* list, size_t start, size_t end, void *(*allocator_p)(size_t));

// Seperated the linked list into a new one based on the given function, allocator_p can be used to send in your own allocator for the returned linked_list struct else it will use the allocator from
// the given linked list
// No new memory besides the new linked list struct is being allocated
// The func should return 1 to be seperated into the new list
linked_list *seperate_linked_list(linked_list *list, int(*func)(void *), void *(*allocator_p)(size_t));

// Merge sorts the linked list, compare function must be set O(n log(n))
void merge_sort_ll(linked_list *list);

// Checks if the linked list is sorted returns 1 if it is, compare function must be set
int is_sorted_ll(linked_list *list);

// Returns the index of a certain values occurence in the linked list, occurence >= 1, returns the size of the linked list if it there is no value at such occurrence
size_t get_index_ll(linked_list *list, void *value, size_t occurrence);

// Copies the entire linked list into an array stored on the heap, need to free both
// Each element of the array is a void * to data
void **get_as_array_ll(linked_list *list);

// Converts the entire linked list into an array stored on the heap, linked list is freed, still need to free array
// Each element of the array is a void * to data
void **convert_to_array_ll(linked_list *list);

// Assumes array is already stored on the heap, frees the array when converting to linked list
linked_list *array_to_linked_list(void **array, size_t size_of_array, size_t size_of_value, void *(*allocator_p)(size_t), void (*freev)(void *));

// Also converts array to linked list but the array is not freed
linked_list *array_as_linked_list(void **array, size_t size_of_array, size_t size_of_value, void *(*allocator_p)(size_t), void (*freev)(void*));

// Prints the function addresses and does a size check and a tail pointer check by iterating over
// the linked list. Prints size and tail address as well.
// Set fix to 1 if you want the function to attempt to fix the linked list else 0
// NOTE, both tests should allways pass if not there is something wrong with your linked list
// it may be from the user or a problem with one of the provided functions
int internal_check_ll(linked_list *list, int fix);

// Attempts to fix the linked list by setting the size to the size the function found when iterating over the linked list as well as setting the tail to what it found
// Set display_fix_message to 1 if you want to see what it did
void fix_ll(linked_list *list, int display_fix_message);

// These are getter methods that returns a copy of the data type stated in the method name, not pointers
// Note these methods do not check types they assume that there is actually the given type there
// If you are unsure if you are getting the correct type do not use these methods below they will crash your program
int get_int_val_ll(linked_list *list, size_t index);

unsigned int get_uint_val_ll(linked_list *list, size_t index);

long get_long_val_ll(linked_list *list, size_t index);

unsigned long get_ulong_val_ll(linked_list *list, size_t index);

short get_short_val_ll(linked_list *list, size_t index);

unsigned short get_ushort_val_ll(linked_list *list, size_t index);

float get_float_val_ll(linked_list *list, size_t index);

double get_double_val_ll(linked_list *list, size_t index);

char get_char_val_ll(linked_list *list, size_t index);

unsigned char get_uchar_val_ll(linked_list *list, size_t index);

char *get_str_val_ll(linked_list *list, size_t index);
#endif
