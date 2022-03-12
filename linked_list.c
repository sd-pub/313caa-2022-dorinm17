// Manea Dorin-Mihai, 313CAa
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

typedef struct ll_node_t
{
    void* data;
    struct ll_node_t* next;
} ll_node_t;

typedef struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

linked_list_t*
ll_create(unsigned int data_size)
{
    linked_list_t* list;
    list = malloc(sizeof(*list));
    DIE(list == NULL, "Memory could not be allocated\n");

    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;

    return list;
}

void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    if (!list)
        return;
    if (n > list->size)
        n = list->size;
    
    ll_node_t* new_node;
    new_node = malloc(sizeof(*new_node));
    DIE(new_node == NULL, "Memory could not be allocated\n");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "Memory could not be allocated\n");
    memcpy(new_node->data, new_data, list->data_size);

    list->size++;

    if (list->size == 1) {
        list->head = new_node;
        new_node->next = NULL;
        return;
    }

    if (n == 0) {
        new_node->next = list->head;
        list->head = new_node;
        return;
    }

    ll_node_t* current = list->head;
    for (int i = 0; i < n - 1; i++)
        current = current->next;
    new_node->next = current->next;
    current->next = new_node;
}

ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    if (!list)
        return NULL;
    if (list->size == 0)
        return NULL;
    if (n + 1 > list->size)
        n = list->size;

    list->size--;

    ll_node_t *current = list->head;
    if (n == 0) {
        list->head = list->head->next;
        return current;
    }

    for (int i = 0; i < n - 1; i++)
        current = current->next;
    
    ll_node_t *removed = current->next;
    current->next = current->next->next;
    return removed;
}

unsigned int
ll_get_size(linked_list_t* list)
{
    if (!list)
        return -1;

    return list->size;
}

void
ll_free(linked_list_t** pp_list)
{
    if (!pp_list)
        return;
    if (!(*pp_list))
        return;

    while (ll_get_size(*pp_list)) {
        ll_node_t *removed = ll_remove_nth_node(*pp_list, 0);
        free(removed->data);
        free(removed);
    }

    free(*pp_list);
    *pp_list = NULL;
}

void
ll_print_int(linked_list_t* list)
{
    if (!list)
        return;

    ll_node_t* current = list->head;
    for (int i = 0; i < list->size; i++) {
        printf("%d ", *((int *)current->data));
        current = current->next;
    }

    printf("\n");
}

void
ll_print_string(linked_list_t* list)
{
    if (!list)
        return;

    ll_node_t* current = list->head;
    for (int i = 0; i < list->size; i++) {
        printf("%s ", (char *)current->data);
        current = current->next;
    }

    printf("\n");
}

int main()
{
    linked_list_t* linkedList;
    int is_int = 0;
    int is_string = 0;

    while (1) {
        char command[16], added_elem[64], *end_ptr;
        long nr, pos;

        scanf("%s", command);

        if (strcmp(command, "create_str") == 0) {
            linkedList = ll_create(MAX_STRING_SIZE);
            is_string = 1;
        }

        if (strcmp(command, "create_int") == 0) {
            linkedList = ll_create(sizeof(int));
            is_int = 1;
        }

        if (strcmp(command, "add") == 0) {
            scanf("%ld", &pos);
            scanf("%s", added_elem);

            nr = strtol(added_elem, &end_ptr, 10);
            if (nr != 0) {
                ll_add_nth_node(linkedList, pos, &nr);
            } else {
                ll_add_nth_node(linkedList, pos, end_ptr);
            }
        }

        if (strcmp(command, "remove") == 0) {
            scanf("%ld", &pos);
            ll_node_t *removed = ll_remove_nth_node(linkedList, pos);
            free(removed->data);
            free(removed);
        }

        if (strcmp(command, "print") == 0) {
            if (is_int == 1) {
                ll_print_int(linkedList);
            }

            if (is_string == 1) {
                ll_print_string(linkedList);
            }
        }

        if (strcmp(command, "free") == 0) {
            ll_free(&linkedList);
            break;
        }
    }

    return 0;
}
