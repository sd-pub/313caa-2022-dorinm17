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
    if (n % list->size <= list->size - n % list->size)
        for (unsigned int i = 0; i < n % list->size; i++)
            curr = curr->next;
    else
        for (unsigned int i = list->size; i > list->size - n % list->size + 1; i--)
            curr = curr->prev;
    return curr;
}

void
dll_add_nth_node(doubly_linked_list_t* list, unsigned int n, const void* data)
{
    if (!list)
        return;
    if (n > list->size)
        n = list->size;

    dll_node_t* new_data;
    new_data = malloc(sizeof(*new_data));
    DIE(new_data == NULL, "Memory could not be allocated\n");
    new_data->data = malloc(list->data_size);
    DIE(new_data->data == NULL, "Memory could not be allocated\n");
    memcpy(new_data->data, data, list->data_size);

    list->size++;
    
    if (list->size == 1) {
        list->head = new_data;
        new_data->next = new_data;
        new_data->prev = new_data;
        return;
    }

    dll_node_t *curr;
    if (n == 0) {
        curr = dll_get_nth_node(list, list->size - 2);
        list->head = new_data;
    } else {
        curr = dll_get_nth_node(list, n - 1);
    }

    new_data->next = curr->next;
    new_data->prev = curr;
    curr->next = new_data;
    new_data->next->prev = new_data;
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

void
dll_print_strings_left_circular(dll_node_t* start)
{
    dll_node_t* curr = start;
    do {
        printf("%s ", (char *)curr->data);
        curr = curr->prev;
    } while (curr != start);

    printf("\n");
}

void
dll_print_string_list(doubly_linked_list_t* list)
{
    if (!list)
        return;

    dll_node_t* curr = list->head;
    for (unsigned int i = 0; i < list->size; i++) {
        curr = curr->prev;
        printf("%s ", (char *)curr->data);
    }

    printf("\n");
}

int main()
{
    doubly_linked_list_t *doublyLinkedList;
    int is_int = 0;
    int is_string = 0;
    while(1) {
        char command[16], added_elem[MAX_STRING_SIZE];
        long nr, pos;
        scanf("%s", command);
        if(strcmp(command, "create_str") == 0){
            doublyLinkedList = dll_create(MAX_STRING_SIZE);
            is_string = 1;
        }
        if(strcmp(command, "create_int") == 0){
            doublyLinkedList = dll_create(sizeof(int));
            is_int = 1;
        }
        if(strcmp(command, "add") == 0){
            scanf("%ld", &pos);

            if(is_int) {
                scanf("%ld", &nr);
                dll_add_nth_node(doublyLinkedList, pos, &nr);
            } else if(is_string) {
                scanf("%s", added_elem);
                dll_add_nth_node(doublyLinkedList, pos, added_elem);
            } else {
                printf("Create a list before adding elements!\n");
                exit(0);
            }
        }
        if(strcmp(command, "remove") == 0){
            if(!is_int && !is_string) {
                printf("Create a list before removing elements!\n");
                exit(0);
            }

            scanf("%ld", &pos);
            dll_node_t* removed = dll_remove_nth_node(doublyLinkedList, pos);
            free(removed->data);
            free(removed);
        }
        if(strcmp(command, "print") == 0){
            if(!is_int && !is_string) {
                printf("Create a list before printing!\n");
                exit(0);
            }

            if(is_int == 1){
                dll_print_int_list(doublyLinkedList);
            }
            if(is_string == 1){
                dll_print_string_list(doublyLinkedList);
            }
        }
        if(strcmp(command, "free") == 0){
            if(!is_int && !is_string) {
                printf("Create a list before freeing!\n");
                exit(0);
            }
            dll_free(&doublyLinkedList);
            break;
        }
    }
    return 0;
}
