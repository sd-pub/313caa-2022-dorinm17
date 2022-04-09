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

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX_STRING_SIZE	256

typedef struct ll_node_t ll_node_t;
struct ll_node_t
{
	void* data;
	ll_node_t* next;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t
{
	ll_node_t* head;
	unsigned int data_size;
	unsigned int size;
};

linked_list_t*
ll_create(unsigned int data_size)
{
	linked_list_t* ll = calloc(1, sizeof(*ll));
	ll->data_size = data_size;

	return ll;
}

static ll_node_t*
get_nth_node(linked_list_t* list, unsigned int n)
{
	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t* node = list->head;

	n = MIN(n, len);

	for (i = 0; i < n; ++i)
		node = node->next;

	return node;
}

static ll_node_t*
create_node(const void* new_data, unsigned int data_size)
{
	ll_node_t* node = calloc(1, sizeof(*node));
	node->data = malloc(data_size);
	memcpy(node->data, new_data, data_size);

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
	ll_node_t *new_node, *prev_node;

	if (!list)
		return;

	new_node = create_node(new_data, list->data_size);

	if (!n || !list->size) {
		new_node->next = list->head;
		list->head = new_node;
	} else {
		prev_node = get_nth_node(list, n - 1);
		new_node->next = prev_node->next;
		prev_node->next = new_node;
	}

	++list->size;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
	ll_node_t *prev_node, *removed_node;

	if (!list || !list->size)
		return NULL;

	if (!n) {
		removed_node = list->head;
		list->head = removed_node->next;
		removed_node->next = NULL;
	} else {
		prev_node = get_nth_node(list, n - 1);
		removed_node = prev_node->next;
		prev_node->next = removed_node->next;
		removed_node->next = NULL;
	}

	--list->size;

	return removed_node;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
ll_get_size(linked_list_t* list)
{
	return !list ? 0 : list->size;
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
	ll_node_t* node;

	if (!pp_list || !*pp_list)
		return;

	while ((*pp_list)->size) {
		node = ll_remove_nth_node(*pp_list, 0);
		free(node->data);
		free(node);
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
	ll_node_t* node = list->head;

	for (; node; node = node->next)
		printf("%d ", *((int*)node->data));
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
	ll_node_t* node = list->head;

	for (; node; node = node->next)
		printf("%s ", (char*)node->data);
	printf("\n");
}

/* Graful este NEORIENTAT */

typedef struct
{
	linked_list_t **neighbors; /* Listele de adiacenta ale grafului */
	int nodes;                 /* Numarul de noduri din graf. */
} list_graph_t;

/**
 * Initializeaza graful cu numarul de noduri primit ca parametru si aloca
 * memorie pentru lista de adiacenta a grafului.
 */
list_graph_t*
lg_create(int nodes)
{
	list_graph_t *graph;
	graph = malloc(sizeof(*graph));
	DIE(graph == NULL, "graph malloc failed\n");

	graph->neighbors = malloc(nodes * sizeof(*graph->neighbors));
	DIE(graph->neighbors == NULL, "graph->neighbors malloc failed\n");
	for (int i = 0; i < nodes; i++)
		graph->neighbors[i] = ll_create(sizeof(int));

	graph->nodes = nodes;
	return graph;
}

/* Adauga o muchie intre nodurile primite ca parametri */
void
lg_add_edge(list_graph_t *graph, int node1, int node2)
{
	if (!graph)
		return;
	if (!graph->neighbors)
		return;
	if (node1 < 0 || node1 >= graph->nodes || node2 < 0 || node2 >= graph->nodes)
		return;
	
	ll_add_nth_node(graph->neighbors[node1], graph->neighbors[node1]->size, &node2);
	ll_add_nth_node(graph->neighbors[node2], graph->neighbors[node2]->size, &node1);
}

/* Returneaza 1 daca exista muchie intre cele doua noduri, 0 in caz contrar */
int
lg_has_edge(list_graph_t *graph, int node1, int node2)
{
	if (!graph)
		return 0;
	if (!graph->neighbors)
		return 0;
	if (node1 < 0 || node1 >= graph->nodes || node2 < 0 || node2 >= graph->nodes)
		return 0;

	ll_node_t *curr_edge = graph->neighbors[node1]->head;
	while (curr_edge) {
		if (*((int *)curr_edge->data) == node2)
			return 1;
		curr_edge = curr_edge->next;
	}
	return 0;
}

/* Elimina muchia dintre nodurile primite ca parametri */
void
lg_remove_edge(list_graph_t *graph, int node1, int node2)
{
	if (!graph)
		return;
	if (!graph->neighbors)
		return;
	if (node1 < 0 || node1 >= graph->nodes || node2 < 0 || node2 >= graph->nodes)
		return;
	if (!lg_has_edge(graph, node1, node2))
		return;

	ll_node_t *curr_edge = graph->neighbors[node1]->head;
	unsigned int index = 0;
	while (*((int *)curr_edge->data) != node2) {
		curr_edge = curr_edge->next;
		index++;
	}
	ll_node_t *removed_edge = ll_remove_nth_node(graph->neighbors[node1], index);
	free(removed_edge->data);
	free(removed_edge);

	curr_edge = graph->neighbors[node2]->head;
	index = 0;
	while (*((int *)curr_edge->data) != node1) {
		curr_edge = curr_edge->next;
		index++;
	}
	removed_edge = ll_remove_nth_node(graph->neighbors[node2], index);
	free(removed_edge->data);
	free(removed_edge);
}

/* Elibereaza memoria folosita de lista de adiacenta a grafului */
void
lg_free(list_graph_t *graph)
{
	if (!graph)
		return;
	if (!graph->neighbors) {
		free(graph);
		return;
	}

	for (int i = 0; i < graph->nodes; i++)
		ll_free(&graph->neighbors[i]);
	free(graph->neighbors);
	free(graph);
}

/* Printeaza lista de adiacenta a grafului
 */
void
lg_print_graph(list_graph_t *graph)
{
	if (!graph)
		return;
	if (!graph->neighbors)
		return;

	for (int i = 0; i < graph->nodes; i++) {
		printf("%d: ", i);
		ll_print_int(graph->neighbors[i]);
	}
}

int main(void)
{

	list_graph_t *lg = NULL;

	while (1) {
		char command[MAX_STRING_SIZE];
		int nr1, nr2;
		int nr_nodes;

		scanf("%s", command);

		if (strncmp(command, "create_lg", 9) == 0) {
			scanf("%d", &nr_nodes);
			lg = lg_create(nr_nodes);
		}

		if (strncmp(command, "add_edge", 8) == 0) {
			if (lg != NULL) {
				scanf("%d %d", &nr1, &nr2);
				lg_add_edge(lg, nr1, nr2);
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
		}

		if (strncmp(command, "remove_edge", 11) == 0) {
			if (lg != NULL) {
				scanf("%d %d", &nr1, &nr2);
				lg_remove_edge(lg, nr1, nr2);
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
		}

		if (strncmp(command, "print_graph", 11) == 0) {
			if (lg != NULL) {
				lg_print_graph(lg);
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
		}

		if (strncmp(command, "has_edge", 5) == 0) {
			if (lg != NULL) {
				int flag;
				scanf("%d %d", &nr1, &nr2);
				flag = lg_has_edge(lg, nr1, nr2);
				if (flag == 1) {
					printf("Has edge\n");
				}
				else if (flag == 0) {
					printf("No edge\n");
				}
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
		}

		if (strncmp(command, "free", 4) == 0) {
			if (lg != NULL) {
				lg_free(lg);
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
			break;
		}
	}

	return 0;
}