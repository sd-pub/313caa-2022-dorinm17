// Manea Dorin-Mihai, 313CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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

#define MAX_STRING_SIZE	256
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/* --- LINKED LIST SUPPORT START --- */

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

linked_list_t *
ll_create(unsigned int data_size)
{
    linked_list_t* ll;

    ll = malloc(sizeof(*ll));

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se returneaza
 * ultimul nod.
*/
ll_node_t* get_nth_node(linked_list_t* list, unsigned int n)
{
    if (!list) {
        return NULL;
    }

    unsigned int len = list->size - 1;
    unsigned int i;
    ll_node_t* node = list->head;

    n = MIN(n, len);

    for (i = 0; i < n; ++i) {
        node = node->next;
    }

    return node;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */
void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }

    /* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    new_node->data = malloc(list->data_size);
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t *
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return NULL;
    }

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
ll_get_size(linked_list_t* list)
{
     if (!list) {
        return -1;
    }

    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void
ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void
ll_print_int(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void
ll_print_string(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
    }

    printf("\n");
}

/* --- START IMPLEMENT STACK HERE --- */

typedef struct stack_t stack_t;
struct stack_t
{
	struct linked_list_t *list;
};

stack_t *
st_create(unsigned int data_size)
{
	stack_t *st;
    st = malloc(sizeof(*st));
    DIE(st == NULL, "st malloc failed\n");

    st->list = ll_create(data_size);
    DIE(st->list ==  NULL, "st->list malloc failed\n");

    return st;
}

/*
 * Functia intoarce numarul de elemente din stiva al carei pointer este trimis
 * ca parametru.
 */
unsigned int
st_get_size(stack_t *st)
{
	if (!st) {
        return 0;
    }
    if (!st->list) {
        return 0;
    }

	return st->list->size;
}

/*
 * Functia intoarce 1 daca stiva al carei pointer este trimis
 * ca parametru este goala si 0 in caz contrar.
 */
unsigned int
st_is_empty(stack_t *st)
{
	if (st_get_size(st))
        return 0;
	return 1;
}

/*
 * Functia intoarce elementul din vârful stivei al carei pointer este trimis
 * ca parametru fără a efectua nicio modificare asupra acesteia.
 */
void *
st_peek(stack_t *st)
{
    if (!st)
	    return NULL;
    if (!st->list)
	    return NULL;
    if (st_is_empty(st))
        return NULL;
    
    return st->list->head->data;
}

/*
 * Functia elimina elementul din vârful stivei al carei pointer este trimis
 * ca parametru. Atentie! Este responsabilitatea acestei functii de a elibera
 * memoria pentru elementul eliminat.
 */
void
st_pop(stack_t *st)
{
	if (!st)
	    return;
    if (!st->list)
	    return;
    if (st_is_empty(st))
        return;

    ll_node_t *removed = ll_remove_nth_node(st->list, 0);
    free(removed->data);
    free(removed);
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se adauga un nou element in
 * vârful stivei al carei pointer este trimis ca parametru.
 */
void
st_push(stack_t *st, void *new_data)
{
	if (!st)
	    return;
    if (!st->list)
	    return;

    ll_add_nth_node(st->list, 0, new_data);
}

/*
 * Functia elimina toate elementele stivei al carei pointer este trimis
 * ca parametru.
 */
void
st_clear(stack_t *st)
{
	while (!st_is_empty(st))
        st_pop(st);
}

/*
 * Functia elibereaza memoria folosita de toate elementele, iar la
 * sfarsit elibereaza memoria folosita de structura stivei.
 */
void
st_free(stack_t *st)
{
    st_clear(st);
    
    free(st->list);
    free(st);
}

int main()
{
    
    stack_t *stack;
    int is_int = 0;
    int is_string = 0;

    while (1) {
        char command[MAX_STRING_SIZE], added_elem[MAX_STRING_SIZE];
        long nr;

        scanf("%s", command);

        if (strncmp(command, "create_stack_str", 16) == 0) {
            stack = st_create(MAX_STRING_SIZE);
            is_string = 1;
        }

        if (strncmp(command, "create_stack_int", 16) == 0) {
            stack = st_create(sizeof(int));
            is_int = 1;
        }

        if (strncmp(command, "push", 4) == 0) {
          
            if(is_int) {
                scanf("%ld", &nr);
                st_push(stack, &nr);
            } else if(is_string) {
                scanf("%s", added_elem);
                st_push(stack, added_elem);
            } else {
                printf("Create a stack first!\n");
                exit(0);
            }
        }

        if (strncmp(command, "pop", 3) == 0) {
            if(!is_int && !is_string) {
                printf("Create a stack first!\n");
                exit(0);
            }

            st_pop(stack);
        }

        if (strncmp(command, "peek", 4) == 0) {
            if(!is_int && !is_string) {
                printf("Create a stack first!\n");
                exit(0);
            }

            if (is_int == 1) {
                int val = *(int *) st_peek(stack);
                printf("%d\n", val);
            }

            if (is_string == 1) {
                char *val_str = (char *) st_peek(stack);
                printf("%s\n", val_str);
            }
        }

        if (strncmp(command, "empty", 5) == 0) {
            if(!is_int && !is_string) {
                printf("Create a stack first!\n");
                exit(0);
            }

            if (st_is_empty(stack)) {
                printf("empty\n");
            } else {
                printf("not empty\n");
            }
        }

        if (strncmp(command, "free", 4) == 0) {
            if(!is_int && !is_string) {
                printf("Create a stack first!\n");
                exit(0);
            }
            st_clear(stack);
            st_free(stack);
            break;
        }
    }

    return 0;
}
