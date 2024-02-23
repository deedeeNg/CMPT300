#include "list.h"
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

// General Error Handling:
// Client code is assumed never to call these functions with a NULL List pointer, or 
// bad List pointer. If it does, any behaviour is permitted (such as crashing).
// HINT: Use assert(pList != NULL); just to add a nice check, but not required.

static Node LIST_OF_AVAILABLE_NODES[LIST_MAX_NUM_NODES];
static List LIST_OF_AVAILABLE_HEADS[LIST_MAX_NUM_HEADS];
static int NODES_INDEX;
static int HEADS_INDEX;
static int dummy = -1;
static enum ListOutOfBounds OOB_START = LIST_OOB_START;
static enum ListOutOfBounds OOB_END = LIST_OOB_END;

Node* getNode(void* item) {
    if (NODES_INDEX == -1) {
        return NULL;
    }

    Node* node = &LIST_OF_AVAILABLE_NODES[NODES_INDEX];
    node->item = item;
    NODES_INDEX = node->nextAvailable;


    return node;
}

List* getList() {
    if (HEADS_INDEX == -1) {
        return NULL;
    }

    List* list = &LIST_OF_AVAILABLE_HEADS[HEADS_INDEX];
    HEADS_INDEX = list->nextAvailable;

    return list;
}

void freeNode(Node* node) {
    node->item = NULL;
    node->next = NULL;
    node->prev = NULL;
    node->nextAvailable = NODES_INDEX;
    NODES_INDEX = node - LIST_OF_AVAILABLE_NODES;
}

void freeList(List* list) {
    list->curr_ptr = NULL;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->nextAvailable = HEADS_INDEX;
    HEADS_INDEX = list - LIST_OF_AVAILABLE_HEADS;
}

// Makes a new, empty list, and returns its reference on success. 
// Returns a NULL pointer on failure.
List* List_create() {
    List* list = getList();
    return list;
}

// Returns the number of items in pList.
int List_count(List* pList) {
    return pList->size;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList) {
    if (pList->size == 0) {
        pList->curr_ptr = NULL;
        return NULL;
    }

    pList->curr_ptr = pList->head;
    return pList->curr_ptr->item;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList) {
    if (pList->size == 0) {
        pList->curr_ptr = NULL;
        return NULL;
    }

    pList->curr_ptr = pList->tail;
    return pList->curr_ptr->item;
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer 
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList) {
    if (pList->curr_ptr == NULL) {
        return NULL;
    }
    if (pList->curr_ptr == pList->tail) {
        pList->state = LIST_OOB_END;
        pList->curr_ptr = NULL;
        return NULL;
    }

    pList->curr_ptr = pList->curr_ptr->next;
    return pList->curr_ptr->item;
}

// Backs up pList's current item by one, and returns a pointer to the new current item. 
// If this operation backs up the current item beyond the start of the pList, a NULL pointer 
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList) {
    if (pList->curr_ptr == NULL) {
        return NULL;
    } 
    if (pList->curr_ptr == pList->head) {
        pList->state = LIST_OOB_START;
        pList->curr_ptr = NULL;
        return NULL;
    }

    pList->curr_ptr = pList->curr_ptr->prev;
    return pList->curr_ptr->item;
}

// Returns a pointer to the current item in pList.
void* List_curr(List* pList) {
    if (pList->curr_ptr == NULL) {
        return NULL;
    }
    
    return pList->curr_ptr->item;
}

// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_after(List* pList, void* pItem) {
    Node* newNode = getNode(pItem);
    if (newNode == NULL) {
        return LIST_FAIL;
    }

    if (pList->size == 0) {
        pList->head = newNode;
        pList->tail = newNode;
        pList->curr_ptr = newNode;
        pList->size++;
        return LIST_SUCCESS;
    }

    if ((pList->curr_ptr == NULL && pList->state == LIST_OOB_START)) {
        newNode->next = pList->head;
        pList->head->prev = newNode;
        pList->head = newNode;
    } else if((pList->curr_ptr == NULL && pList->state == LIST_OOB_END)) {
        newNode->prev = pList->tail;
        pList->tail->next = newNode;
        pList->tail = newNode;
    } else {
        newNode->prev = pList->curr_ptr;
        newNode->next = pList->curr_ptr->next;
        if (pList->curr_ptr->next != NULL) {
            pList->curr_ptr->next->prev = newNode;
        } else {
            pList->tail = newNode;
        }

        pList->curr_ptr->next = newNode;
    }
    pList->curr_ptr = newNode;
    pList->size++;
    return LIST_SUCCESS;
}

// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_before(List* pList, void* pItem) {
    Node* newNode = getNode(pItem);
    if (newNode == NULL) {
        return LIST_FAIL;
    }

    if (pList->size == 0) {
        pList->head = newNode;
        pList->tail = newNode;
        pList->curr_ptr = newNode;
        pList->size++;
        return LIST_SUCCESS;
    }

    if ((pList->curr_ptr == NULL && pList->state == LIST_OOB_START)) {
        newNode->next = pList->head;
        pList->head->prev = newNode;
        pList->head = newNode;
    } else if((pList->curr_ptr == NULL && pList->state == LIST_OOB_END)) {
        newNode->prev = pList->tail;
        pList->tail->next = newNode;
        pList->tail = newNode;
    } else {
        newNode->prev = pList->curr_ptr->prev;
        newNode->next = pList->curr_ptr;
        if (pList->curr_ptr->prev != NULL) {
            pList->curr_ptr->prev->next = newNode;
        } else {
            pList->head = newNode;
        }

        pList->curr_ptr->prev = newNode;
    }
    pList->curr_ptr = newNode;
    pList->size++;
    return LIST_SUCCESS;
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem) {
    Node* newNode = getNode(pItem);
    if (newNode == NULL) {
        return LIST_FAIL;
    }

    if (pList->size == 0) {
        pList->head = newNode;
        pList->tail = newNode;
        pList->curr_ptr = newNode;
        pList->size++;
        return LIST_SUCCESS;
    }

    newNode->next = NULL;
    newNode->prev = pList->tail;
    if (pList->tail != NULL) {
        pList->tail->next = newNode;
    }

    pList->tail = newNode;
    pList->curr_ptr = newNode;
    pList->size++;
    return LIST_SUCCESS;
}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem) {
    Node* newNode = getNode(pItem);
    if (newNode == NULL) {
        return LIST_FAIL;
    }

    if (pList->size == 0) {
        pList->head = newNode;
        pList->tail = newNode;
        pList->curr_ptr = newNode;
        pList->size++;
        return LIST_SUCCESS;
    }

    newNode->next = pList->head;
    newNode->prev = NULL;
    if (pList->head != NULL) {
        pList->head->prev = newNode;
    }

    pList->head = newNode;
    pList->curr_ptr = newNode;
    pList->size++;
    return LIST_SUCCESS;
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList) {
    if (pList->curr_ptr == NULL) {
        return NULL;
    }

    void* item = pList->curr_ptr->item;
    Node* currNode = pList->curr_ptr;

    if (pList->curr_ptr->next != NULL) {
        pList->curr_ptr->next->prev = pList->curr_ptr->prev;
    } else {
        pList->tail = pList->tail->prev;
    }

    if (pList->curr_ptr->prev != NULL) {
        pList->curr_ptr->prev->next = pList->curr_ptr->next;
    } else {
        pList->head = pList->head->next;
    }

    pList->curr_ptr = pList->curr_ptr->next;
    pList->size--;
    freeNode(currNode);
    return item;
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList) {
    if (pList == NULL || pList->size == 0) {
        return NULL;
    }

    pList->curr_ptr = pList->tail;
    void* item = List_remove(pList);
    pList->curr_ptr = pList->tail;
    return item;
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2) {
    if (pList1 == NULL || pList2 == NULL) {
        return;
    }

    if (pList1->tail == NULL) {
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
    } else if(pList2->head != NULL){
        pList1->tail->next = pList2->head;
        pList2->head->prev = pList1->tail;
        pList1->tail = pList2->tail;
    }

    pList1->size += pList2->size;

    freeList(pList2);
}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
typedef void (*FREE_FN)(void* pItem);
void List_free(List* pList, FREE_FN pItemFreeFn) {
    List_first(pList);
    while (pList->curr_ptr != NULL) {
        void* item = List_trim(pList);
        pItemFreeFn(item);
    }

    freeList(pList);
}

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
    if (pList->curr_ptr == NULL && pList->state == LIST_OOB_START) {
        pList->curr_ptr = pList->head;
    }

    while(pList->curr_ptr != NULL) {
        if (pComparator(pComparisonArg, pList->curr_ptr->item)) {
            void* item = pList->curr_ptr->item;
            List_prev(pList);
            return item;
        }
        pList->curr_ptr = pList->curr_ptr->next;
    }

    pList->state = LIST_OOB_END;
    return NULL;
}
