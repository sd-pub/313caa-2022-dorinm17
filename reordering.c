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

// similar to QuickSort partitioning

// the list will be structured into 3 zones while partitioning:
// I. list(i) <= x; II. list(i) > x; III. elements to be checked

// if the current element to be checked is > x, it lies already in zone II
// and we move on to the next element

// if the current element to be checked is <= x, the elements in zone II need
// to be shifted one element to the right, to make room for the new element in
// zone I

void
partition(linked_list_t* list, int x)
{
    if (!list)
        return;
    if (list->size == 0)
        return;

    ll_node_t* current1 = list->head;
    ll_node_t* current2 = list->head;
    int j = 0;
    
    for (int i = 0; i < list->size; i++) {
        int* data = current2->data;
        if (*data <= x) {
            int* aux = current1->data;
            current1->data = data;

            ll_node_t *temp = current1;
            for (int k = j; k < i; k++) {
                int* aux1 = temp->next->data;
                temp->next->data = aux;
                aux = aux1;
                temp = temp->next;
            }

            current1 = current1->next;
            j++;
        }
        current2 = current2->next;
    }
}

int main()
{
    linked_list_t* linkedList;

    while (1) {
        char command[16];
        long size, num;

        scanf("%s", command);

        if (strcmp(command, "list") == 0) {
            linkedList = ll_create(sizeof(int));
            scanf("%ld", &size);

            long int curr_nr;
            for (int i = 0; i < size; ++i) {
                scanf("%ld", &curr_nr);
                ll_add_nth_node(linkedList, size, &curr_nr);
            }
        }

        if (strcmp(command, "X") == 0) {
            scanf("%ld", &num);

            partition(linkedList, num);
            ll_print_int(linkedList);
            break;
        }
    }

    ll_free(&linkedList);

    return 0;
}
