#include "linked_list.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void print_as_int(void *value) {
	printf("Integer: %d\n", *(int*)value);
}

int remove_all_ll(void *placeholder) {
	return 1;
}

int remove_odd_values(void *data) {
	return (*(int *)data) % 2;
}

int compare_int(const void *a, const void *b) {
	int aa = *(int*)(a);
	int bb = *(int*)(b);

	if (aa > bb) {
		return 1;
	} else if (aa == bb) {
		return 0;
	} else {
		return -1;
	}
}


int main() {
	int values[] = {1, 2, 3, 4, 5, 6, 7, 8};
	linked_list *my_list = new_linked_list(NULL);
	set_print_ll(my_list, print_as_int);
	int i = 0;
	printf("----- ----- Adding Test ----- -----\n");
	for (i = 0; i < 8; i++) {
		printf("Int added at index: %zu\n", append_ll(my_list, &values[i], sizeof(int)));
	}
	print_ll(my_list);
	printf("\nCurrent Size: %zu\n", get_size_ll(my_list));
	printf("----- ----- Removing Test ----- -----\n");
	for (i = 0; i < 11; i += 2) {
		printf("Attempt to remove data at index: %d, result: %d\n", i, delete_ll(my_list, i));
	}
	print_ll(my_list);
	printf("\nCurrent Size: %zu\n", get_size_ll(my_list));
	printf("----- Two threes added at front -----\n");
	prepend_ll(my_list, &values[2], sizeof(int));
	prepend_ll(my_list, &values[2], sizeof(int));
	print_ll(my_list);
	printf("\nCurrent Size: %zu\n", get_size_ll(my_list));
	insert_ll(my_list, &values[6], sizeof(int), 5);
	insert_ll(my_list, &values[6], sizeof(int), 7);
	printf("----- Inserted 7 at index 5 and 7 -----\n");
	print_ll(my_list);
	printf("\nCurrent Size: %zu\n", get_size_ll(my_list));
	linked_list *clone_list = clone_linked_list(my_list, NULL);
	delete_ll(clone_list, 0);
	printf("----- ----- Cloned List (first element removed) ----- -----\n");
	print_ll(clone_list);
	printf("\nCurrent Clone Size: %zu\n", get_size_ll(clone_list));
	printf("----- ----- Reverse Linked List Test ----- -----\n");
	reverse_ll(clone_list);
	print_ll(clone_list);
	printf("\n----- ----- Getting Values Test (from cloned list) ----- -----\n");
	printf("Index 0: %d\n", get_int_val_ll(clone_list, 0));
	printf("Index 5: %d\n", get_int_val_ll(clone_list, 5));
	printf("\n----- ----- Extraction Test ----- -----\n");
	printf("----- my_list before extractions -----\n");
	print_ll(my_list);
	void *extract1 = extract_ll(my_list, 3);
	void *extract2 = extract_head_ll(my_list);
	void *extract3 = extract_ll(my_list, 6);
	printf("Extracted from index 3: %d\n", *(int*)extract1);
	printf("Extracted from head: %d\n", *(int*)extract2);
	printf("Extracted from tail (index 6) %d\n", *(int*)extract3);
	printf("----- my list after extractions -----\n");
	print_ll(my_list);
	printf("\n----- ----- Filtering tests on cloned list ----- -----\n");
	printf("----- Current Cloned List -----\n");
	print_ll(clone_list);
	printf("----- Cloned List after odd numbers were filtered -----\n");
	filter_ll(clone_list, remove_odd_values);
	print_ll(clone_list);
	printf("----- Cloned List after all values were filtered -----\n");
	filter_ll(clone_list, remove_all_ll);
	print_ll(clone_list);
	printf("----- ----- Re populating cloned list ----- -----\n");
	for (i = 0; i < 8; i++) {
		printf("Int added at index: %zu\n", append_ll(clone_list, &values[i], sizeof(int)));
	}
	print_ll(clone_list);
	printf("----- Slicing cloned list from 1 - 4 and printing it-----");
	linked_list *sliced_list = slice_linked_list(clone_list, 1, 4, NULL);
	linked_list *head_slice = slice_linked_list(clone_list, 0, 0, NULL);
	linked_list *tail_slice = slice_linked_list(clone_list, get_size_ll(clone_list) - 1, get_size_ll(clone_list) - 1, NULL);
	print_ll(sliced_list);
	printf("----- Sliced head printed below -----\n");
	print_ll(head_slice);
	printf("----- Sliced tail printed below -----\n");
	print_ll(tail_slice);
	printf("----- ----- Cloned list after the slices ----- -----\n");
	print_ll(clone_list);
	printf("----- Testing iter_ll by adding up everything in the sliced_list -----\n");
	iter_ll(sliced_list);
	// i defined above
	int total = 0;
	void *ret;
	while (i = ((ret = iter_ll(NULL)) ? *(int*)ret: 0)) {
		total += i;
	}
	printf("Total of sliced_list = %d\n", total);

	internal_check_ll(my_list, 0);
	void **my_array = get_as_array_ll(my_list);
	for (i = 0; i < get_size_ll(my_list); i++) {
		printf("Elem %d is %d\n", i + 1, *(int*)(my_array[i]));
	}
	
	free(extract1);
	free(extract2);
	free(extract3);
	//for (int i = 0; i < 6; i++) {
	//	free(my_array[i]);
	//}
	//free(my_array);
	
	void **my_array2 = convert_to_array_ll(my_list);
	for (i = 0; i < 6; i++) {
		printf("(2) Elem %d is %d\n", i + 1, *(int*)(my_array2[i]));
	}


	my_list = array_to_linked_list(my_array, 6, 4, NULL, NULL);
	printf("Test of value at index 0 -> %d\n", get_int_val_ll(my_list, 0));
	printf("Test of value at index 5 -> %d\n", get_int_val_ll(my_list, 5));
	internal_check_ll(my_list, 0);


	linked_list *my_list2 = array_as_linked_list(my_array2, 6, 4, NULL, NULL);
	printf("(2) LL Test of value at index 0 -> %d\n", get_int_val_ll(my_list2, 0));
	printf("(2) LL Test of value at index 5 -> %d\n", get_int_val_ll(my_list2, 5));
	printf("Array Test of value at index 0 -> %d\n", *(int*)my_array2[0]);
	printf("Array Test of value at index 5 -> %d\n", *(int*)my_array2[5]);
	internal_check_ll(my_list2, 0);

	free_linked_list(my_list2);

	for (i = 0; i < 6; i++) {
		free(my_array2[i]);
	}

	clock_t start, end;
	linked_list* five_mil_test = new_linked_list(NULL);

	start = clock();
	for (i = 0; i < 5000000; i++) {// 5 million elements test
		prepend_ll(five_mil_test, &i, sizeof(int));
	}
	end = clock();
	double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("%f Seconds used by the CPU to prepend 5 million elements to a empty linked_list!\n", cpu_time_used);
	
	int ints[22] = {8, 2, 6, 2, 5, 9, 0, -3, -4, 2, 7, 8, 8, 2, 4, 3, 5, 6, 18, 2, 14, 6};
	
	set_compare_ll(my_list, compare_int);
	set_print_ll(my_list, print_as_int);
	for (i = 0; i < 22; i++) {
		prepend_ll(my_list, &ints[i], sizeof(int));
	}

	//linked_list *other_clone = clone_linked_list(my_list, NULL);

	print_ll(my_list);
	merge_sort_ll(my_list);
	internal_check_ll(my_list, 0);
	printf("Merge Sorted linked_list!!!!\n");
	print_ll(my_list);
	printf("Is linked list sorted? %d\n", is_sorted_ll(my_list));

	//print_linked_list(other_clone);
	//quick_sort_ll(other_clone);
	//internal_check_ll(other_clone, 0);
	//printf("Quick Sorted linked_list!!!!\n");
	//print_linked_list(other_clone);
	
	linked_list* list_to_sort = new_linked_list(NULL);
	set_compare_ll(list_to_sort, compare_int);
	set_print_ll(list_to_sort, print_as_int);
	srand(time(NULL));
	int temp;
	for (i = 0; i < 5000000; i++) {
		temp = rand() % 5000000;
		prepend_ll(list_to_sort, &temp, sizeof(int));
	}
	printf("Elements appended to linked list!\n");
	start = clock();
	merge_sort_ll(list_to_sort);
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("%f Seconds used by CPU to sort a 5 million random elements linked list (merge sort)!\n", cpu_time_used);
	internal_check_ll(list_to_sort, 0);

	int seperate_test_func(void * a) {
		int aa = *(int*)a;
		return (aa == -4) || (aa == 18) || (aa == 6);
	}

	linked_list *seperated = seperate_linked_list(my_list, seperate_test_func, NULL);

	printf("my_list\n");
	print_ll(my_list);
	printf("seperated\n");
	print_ll(seperated);

	internal_check_ll(my_list, 0);
	internal_check_ll(seperated, 0);

	combine_ll(my_list, seperated);
	print_ll(my_list);
	internal_check_ll(my_list, 0);
	
	//free_linked_list(other_clone);
	free_linked_list(list_to_sort);
	free(my_array2);
	free_linked_list(five_mil_test);
	//free_linked_list(seperated);
	free_linked_list(my_list);
	free_linked_list(head_slice);
	free_linked_list(tail_slice);
	free_linked_list(sliced_list);
	free_linked_list(clone_list);
	return 0;
}
