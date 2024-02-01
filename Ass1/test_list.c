#include <stdio.h>
#include "list.h"
#include "list.c"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


/*
 *  thhis function's for testing List_search, compare two pointers
 */
bool compareItem(void* pItem1, void* pItem2) {
    return (pItem1 == pItem2);
}

/*
 *  this function's for testing List_free, set the value of the pointer of the item into dummy = -1
 */
void freeItem(void* item) {
    memcpy(item, &dummy, sizeof(int));
}

/*
 *  this function's for setting up data structure
 */
void initialize() {
    for (int i = 0; i < LIST_MAX_NUM_HEADS - 1; i++) {
        LIST_OF_AVAILABLE_HEADS[i].nextAvailable = i + 1;
    }
    LIST_OF_AVAILABLE_HEADS[LIST_MAX_NUM_HEADS - 1].nextAvailable = -1;

    for (int i = 0; i < LIST_MAX_NUM_NODES - 1; i++) {
        LIST_OF_AVAILABLE_NODES[i].nextAvailable = i + 1;
    }
    LIST_OF_AVAILABLE_NODES[LIST_MAX_NUM_NODES - 1].nextAvailable = -1;
}

/*
 * This function's for printing all pointer of item in the list
 */
void dumpList(List* list)
{
    Node* head = list->head;
	while (head != NULL) {
        printf("%p\n", head->item);
        head = head->next;
    }
    printf("And current pointer is: %p\n", List_curr(list));
    printf("\n");
}

/*
 * Main()
 */
int main(int argc, char** argv)
{
    // Set up data structure
    initialize();

    // Creating two lists
    List* list1 = List_create();
    List* list2 = List_create();

    //List of items
    int item[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    List_append(list1, &item[0]);  // list1: {3e0}   curr_ptr: 3e0
    printf("list1 after append item[0]:\n");
    dumpList(list1); 

    List_append(list1, &item[1]); // list1: {3e0, 3e4}   curr_ptr: 3e4
    printf("list1 after append item[1]\n");
    dumpList(list1); 
 
    List_prepend(list1, &item[2]); // list1: {3e8, 3e0, 3e4}   curr_ptr: 3e8
    printf("list1 after prepend item[2]\n");
    dumpList(list1); 

    printf("the next item of list1 is: %p\n", List_next(list1));// list1: {3e8, 3e0, 3e4}   curr_ptr: 3e0
    
    List_insert_after(list1, &item[3]);
    printf("list1 after insert after item[3]\n"); // list1: {3e8, 3e0, 3ec, 3e4}   curr_ptr: 3ec
    dumpList(list1); 

    List_insert_before(list1, &item[4]);
    printf("list1 after insert before item[4]\n"); // list1: {3e8, 3e0, ,3f0, 3ec, 3e4}   curr_ptr: 3f0
    dumpList(list1);  

    printf("the previous item of list1 is: %p\n", List_prev(list1)); // list1: {3e8, 3e0, 3f0, 3ec, 3e4}   curr_ptr: 3e0

    void* remove = List_remove(list1);
    printf("list1 after remove current item\n"); // list1: {3e8, 3f0, 3ec, 3e4}   curr_ptr: 3f0
    printf("the item that has been removed from list1 is: %p\n", remove);
    dumpList(list1); 

    void* trim = List_trim(list1);
    printf("list1 after trim current item\n"); // list1: {3e8, 3f0, 3ec}   curr_ptr: 3ec
    printf("the item that has been trimmed from list1 is: %p\n", trim);
    dumpList(list1); 

    printf("the first item of list1 is: %p\n", List_first(list1)); // list1: {3e8, 3f0, 3ec}   curr_ptr: 3e8
    printf("the last item of list1 is: %p\n", List_last(list1)); // list1: {3e8, 3f0, 3ec}   curr_ptr: 3ec
    printf("the next item of list1 is: %p\n", List_next(list1)); // list1: {3e8, 3f0, 3ec}   curr_ptr: NULL  state: LIST_OOB_END
    printf("size of list1 is: %d\n", List_count(list1));

    // Append 3 items to list2
    for (int i = 5 ; i < 8; i++) {
        List_append(list2, &item[i]);  //list2: {3f4, 3f8, 3fc}    curr_ptr: 40c
    }

    // Merging list2 to the end of list1
    List_concat(list1, list2);
    printf("list1 after concat with list2\n"); // list1: {3e8, 3f0, 3ec, 3f4, 3f8, 3fc}   curr_ptr: NULL  state: LIST_OOB_END   list2: {}
    dumpList(list1);

    printf("the first item of list1 is: %p\n", List_first(list1)); // list1: {3e8, 3f0, 3ec, 3f4, 3f8, 3fc}   curr_ptr: NULL

    printf("searching item[3] in list1: %p\n", List_search(list1, compareItem, &item[3])); // list1: {3e8, 3f0, 3ec, 3f4, 3f8, 3fc}   curr_ptr: 3f0
    printf("searching item[3] in list1: %p\n", List_search(list1, compareItem, &item[2])); // list1: {3e8, 3f0, 3ec, 3f4, 3f8, 3fc}   curr_ptr: NULL state: LIST_OOB_START

    List_free(list1, freeItem);
    printf("list1 after free the list: \n");
    dumpList(list1);

    printf("item after free list1: \n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", item[i]);  // item: 0 1 -1 -1 -1 -1 -1 -1 8 9
    }
    return 0;
}


