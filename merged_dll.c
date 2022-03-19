// Manea Dorin-Mihai, 313CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_STRING_SIZE 64

/* useful macro for handling error codes */
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

typedef struct dll_node_t
{
    void* data;
    struct dll_node_t *prev, *next;
} dll_node_t;

typedef struct doubly_linked_list_t
{
    dll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} doubly_linked_list_t;

doubly_linked_list_t*
dll_create(unsigned int data_size)
{
    doubly_linked_list_t* list;
    list = malloc(sizeof(*list));
    DIE(list == NULL, "Memory could not be allocated\n");

    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;

    return list;
}

dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    if (!list)
        return NULL;
    if (!list->head)
        return NULL;
    if (list->size == 0)
        return list->head;

    dll_node_t* curr = list->head;
    //if (n % list->size <= list->size - n % list->size)
        for (unsigned int i = 0; i < n % list->size; i++)
            curr = curr->next;
    // else
    //     for (unsigned int i = list->size; i > list->size - n % list->size + 1; i--)
    //         curr = curr->prev;
    return curr;
}

void
dll_add_nth_node(doubly_linked_list_t* list, const void* data)
{
    if (!list)
        return;

    dll_node_t* new_data;
    new_data = malloc(sizeof(*new_data));
    DIE(new_data == NULL, "Memory could not be allocated\n");
    new_data->data = malloc(list->data_size);
    DIE(new_data->data == NULL, "Memory could not be allocated\n");
    memcpy(new_data->data, data, list->data_size);

    list->size++;

    if (list->size == 1) {
        new_data->next = NULL;
        new_data->prev = NULL;
        list->head = new_data;
        return;
    }

    dll_node_t* curr = dll_get_nth_node(list, list->size - 2);
    new_data->next = NULL;
    new_data->prev = curr;
    curr->next = new_data;
}

dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    if (!list)
        return NULL;
    if (!list->head)
        return NULL;
    if (n > list->size - 1)
        n = list->size - 1;
    
    list->size--;

    dll_node_t *curr;
    if (n == 0) {
        if (list->size == 0)
            curr = dll_get_nth_node(list, 0);
        else
            curr = dll_get_nth_node(list, list->size - 1);
        list->head = list->head->next;
    } else
        curr = dll_get_nth_node(list, n - 1);

    dll_node_t* removed = curr->next;
    curr->next = removed->next;
    removed->next->prev = curr;
    return removed;
}

unsigned int
dll_get_size(doubly_linked_list_t* list)
{
    if (!list)
        return -1;
    
    return list->size;
}

void
dll_free(doubly_linked_list_t** pp_list)
{
    if (!pp_list)
        return;
    if (!(*pp_list))
        return;
    
    while (dll_get_size(*pp_list) > 0) {
        dll_node_t* removed = dll_remove_nth_node(*pp_list, 0);
        free(removed->data);
        free(removed);
    }

    free(*pp_list);
    *pp_list = NULL;
}

void
dll_print_int_list(doubly_linked_list_t* list)
{
    if (!list)
        return;

    dll_node_t* curr = list->head;
    for (unsigned int i = 0; i < list->size; i++) {
        printf("%d ", *((int *)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

doubly_linked_list_t*
merge_sorted_lists(doubly_linked_list_t* list1, doubly_linked_list_t* list2)
{
    doubly_linked_list_t* mergedList = dll_create(sizeof(int));
    dll_node_t *p = list1->head, *q = list2->head; 

    while (p && q) {
        if (*((int *)p->data) <= *((int *)q->data)) {
            dll_add_nth_node(mergedList, p->data);
            p = p->next;
        } else {
            dll_add_nth_node(mergedList, q->data);
            q = q->next;
        }
    }

    while (p) {
        dll_add_nth_node(mergedList, p->data);
        p = p->next;
    }
    while (q) {
        dll_add_nth_node(mergedList, q->data);
        q = q->next;
    }

    return mergedList;
}

int main()
{
    doubly_linked_list_t *list1, *list2, *mergedlist;
    long size1, size2, curr_nr;

    list1 = dll_create(sizeof(int));
    list2 = dll_create(sizeof(int));

    scanf("%ld", &size1);

    for (int i = 0; i < size1; ++i) {
        scanf("%ld", &curr_nr);
        dll_add_nth_node(list1, &curr_nr);
    }

    scanf("%ld", &size2);

    for (int i = 0; i < size2; ++i) {
        scanf("%ld", &curr_nr);
        dll_add_nth_node(list2, &curr_nr);
    }


    mergedlist = merge_sorted_lists(list1, list2);
    dll_print_int_list(mergedlist);
    
    return 0;
}
