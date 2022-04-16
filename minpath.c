#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define DIE(assertion, call_description)            \
    do                                              \
    {                                               \
        if (assertion)                              \
        {                                           \
            fprintf(stderr, "(%s, %d): ", __FILE__, \
                    __LINE__);                      \
            perror(call_description);               \
            exit(errno);                            \
        }                                           \
    } while (0)

#define MAX_QUEUE_SIZE 100

/* Helper data structures definitions */
typedef struct ll_node_t ll_node_t;
struct ll_node_t
{
    void *data;
    ll_node_t *next;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t
{
    ll_node_t *head;
    unsigned int data_size;
    unsigned int size;
};

typedef struct queue_t queue_t;
struct queue_t
{
    unsigned int max_size;
    unsigned int size;
    unsigned int data_size;
    unsigned int read_idx;
    unsigned int write_idx;
    void **buff;
};

typedef struct
{
    linked_list_t **neighbors;
    int nodes;
} list_graph_t;

/* Helper data structures functions */
linked_list_t *
ll_create(unsigned int data_size)
{
    linked_list_t *ll = calloc(1, sizeof(*ll));
    DIE(!ll, "calloc list");

    ll->data_size = data_size;

    return ll;
}

static ll_node_t *
get_nth_node(linked_list_t *list, unsigned int n)
{
    unsigned int len = list->size - 1;
    unsigned int i;
    ll_node_t *node = list->head;

    n = MIN(n, len);

    for (i = 0; i < n; ++i)
        node = node->next;

    return node;
}

static ll_node_t *
create_node(const void *new_data, unsigned int data_size)
{
    ll_node_t *node = calloc(1, sizeof(*node));
    DIE(!node, "calloc node");

    node->data = malloc(data_size);
    DIE(!node->data, "malloc data");

    memcpy(node->data, new_data, data_size);

    return node;
}

void ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data)
{
    ll_node_t *new_node, *prev_node;

    if (!list)
        return;

    new_node = create_node(new_data, list->data_size);

    if (!n || !list->size)
    {
        new_node->next = list->head;
        list->head = new_node;
    }
    else
    {
        prev_node = get_nth_node(list, n - 1);
        new_node->next = prev_node->next;
        prev_node->next = new_node;
    }

    ++list->size;
}

ll_node_t *
ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
    ll_node_t *prev_node, *removed_node;

    if (!list || !list->size)
        return NULL;

    if (!n)
    {
        removed_node = list->head;
        list->head = removed_node->next;
        removed_node->next = NULL;
    }
    else
    {
        prev_node = get_nth_node(list, n - 1);
        removed_node = prev_node->next;
        prev_node->next = removed_node->next;
        removed_node->next = NULL;
    }

    --list->size;

    return removed_node;
}

unsigned int
ll_get_size(linked_list_t *list)
{
    return !list ? 0 : list->size;
}

void ll_free(linked_list_t **pp_list)
{
    ll_node_t *node;

    if (!pp_list || !*pp_list)
        return;

    while ((*pp_list)->size)
    {
        node = ll_remove_nth_node(*pp_list, 0);
        free(node->data);
        free(node);
    }

    free(*pp_list);
    *pp_list = NULL;
}

void ll_print_int(linked_list_t *list)
{
    ll_node_t *node = list->head;

    for (; node; node = node->next)
        printf("%d ", *(int *)node->data);
    printf("\n");
}

void ll_print_string(linked_list_t *list)
{
    ll_node_t *node = list->head;

    for (; node; node = node->next)
        printf("%s ", (char *)node->data);
    printf("\n");
}

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

queue_t *
q_create(unsigned int data_size, unsigned int max_size)
{
    queue_t *q = calloc(1, sizeof(*q));
    DIE(!q, "calloc queue failed");

    q->data_size = data_size;
    q->max_size = max_size;

    q->buff = malloc(max_size * sizeof(*q->buff));
    DIE(!q->buff, "malloc buffer failed");

    return q;
}

unsigned int
q_get_size(queue_t *q)
{
    return !q ? 0 : q->size;
}

unsigned int
q_is_empty(queue_t *q)
{
    return !q ? 1 : !q->size;
}

void *
q_front(queue_t *q)
{
    if (!q || !q->size)
        return NULL;

    return q->buff[q->read_idx];
}

int q_dequeue(queue_t *q)
{
    if (!q || !q->size)
        return 0;

    free(q->buff[q->read_idx]);

    q->read_idx = (q->read_idx + 1) % q->max_size;
    --q->size;
    return 1;
}

int q_enqueue(queue_t *q, void *new_data)
{
    void *data;
    if (!q || q->size == q->max_size)
        return 0;

    data = malloc(q->data_size);
    DIE(!data, "malloc data failed");
    memcpy(data, new_data, q->data_size);

    q->buff[q->write_idx] = data;
    q->write_idx = (q->write_idx + 1) % q->max_size;
    ++q->size;

    return 1;
}

void q_clear(queue_t *q)
{
    unsigned int i;
    if (!q || !q->size)
        return;

    for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
        free(q->buff[i]);

    q->read_idx = 0;
    q->write_idx = 0;
    q->size = 0;
}

void q_free(queue_t *q)
{
    if (!q)
        return;

    q_clear(q);
    free(q->buff);
    free(q);
}

static int is_node_in_graph(int n, int nodes)
{
    return n >= 0 && n < nodes;
}

list_graph_t *
lg_create(int nodes)
{
    int i;

    list_graph_t *g = malloc(sizeof(*g));
    DIE(!g, "malloc graph failed");

    g->neighbors = malloc(nodes * sizeof(*g->neighbors));
    DIE(!g->neighbors, "malloc neighbours failed");

    for (i = 0; i != nodes; ++i)
        g->neighbors[i] = ll_create(sizeof(int));

    g->nodes = nodes;

    return g;
}

void lg_add_edge(list_graph_t *graph, int src, int dest)
{
    if (
        !graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) || !is_node_in_graph(dest, graph->nodes))
        return;

    ll_add_nth_node(graph->neighbors[src], graph->neighbors[src]->size, &dest);
}

static ll_node_t *find_node(linked_list_t *ll, int node, unsigned int *pos)
{
    ll_node_t *crt = ll->head;
    unsigned int i;

    for (i = 0; i != ll->size; ++i)
    {
        if (node == *(int *)crt->data)
        {
            *pos = i;
            return crt;
        }

        crt = crt->next;
    }

    return NULL;
}

int lg_has_edge(list_graph_t *graph, int src, int dest)
{
    unsigned int pos;

    if (
        !graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) || !is_node_in_graph(dest, graph->nodes))
        return 0;

    return find_node(graph->neighbors[src], dest, &pos) != NULL;
}

linked_list_t *
lg_get_neighbours(list_graph_t *graph, int node)
{
    if (
        !graph || !graph->neighbors || !is_node_in_graph(node, graph->nodes))
        return NULL;

    return graph->neighbors[node];
}

void lg_remove_edge(list_graph_t *graph, int src, int dest)
{
    unsigned int pos;

    if (
        !graph || !graph->neighbors || !is_node_in_graph(src, graph->nodes) || !is_node_in_graph(dest, graph->nodes))
        return;

    if (!find_node(graph->neighbors[src], dest, &pos))
        return;

    ll_remove_nth_node(graph->neighbors[src], pos);
}

void lg_free(list_graph_t *graph)
{
    int i;

    for (i = 0; i != graph->nodes; ++i)
        ll_free(graph->neighbors + i);

    free(graph->neighbors);
    free(graph);
}

/*
    TODO

    Prints the nodes which make up the minimum path between src and dest.

    For example, if the minimum path between A and B is A->C->D->E->F->B, then
    this function will print "A C D E F B".

    Output format:

    node_1 node_2 node_3 ... node_k

    For the purpose of finding minimum paths, the given graph
    can be either oriented or unoriented. For this exercise, the graph
    is oriented.
*/

void
bfs_list_graph(list_graph_t *lg, int node, int *color, int *parent, int *dist, int dest)
{
	queue_t *q = q_create(sizeof(int), lg->nodes - 1);
	q_enqueue(q, &node);
	color[node] = 1;
    dist[node] = 0;

	while (!q_is_empty(q)) {
		int curr_node = *((int *)q_front(q));
		color[curr_node] = 2;
		q_dequeue(q);

        if (curr_node == dest) {
            q_free(q);
            return;
        }

		ll_node_t *curr_neighbor = lg->neighbors[curr_node]->head;
		while (curr_neighbor) {
			int data = *((int *)curr_neighbor->data);
			if (color[data] == 0) {
				parent[data] = curr_node;
				color[data] = 1;
                dist[data] = dist[curr_node] + 1;
				q_enqueue(q, &data);
			}
			
			curr_neighbor = curr_neighbor->next;
		}
	}

	q_free(q);
}

void print_min_path(list_graph_t *graph, int src, int dest)
{
    int *color, *parent, *dist;
    color = calloc(graph->nodes, sizeof(*color));
    DIE(!color, "color malloc failed\n");
    parent = malloc(graph->nodes * sizeof(*parent));
    DIE(!parent, "parent malloc failed\n");
    dist = malloc(graph->nodes * sizeof(*dist));
    DIE(!dist, "dist malloc failed\n");

    for (int i = 0; i < graph->nodes; i++) {
        parent[i] = 1000;
        dist[i] = 1000;
    }

    bfs_list_graph(graph, src, color, parent, dist, dest);

    if (dist[dest] == 1000) {
        printf("No path found\n");
        return;
    }

    stack_t *st = st_create(sizeof(int));
    int curr = dest;
    while (curr != 1000) {
        st_push(st, &curr);
        curr = parent[curr];
    }

    while (!st_is_empty(st)) {
        printf("%d ", *((int *)st_peek(st)));
        st_pop(st);
    }
    printf("\n");

    st_free(st);
    free(color);
    free(parent);
    free(dist);
}

int main()
{
    int n, m, src, dest;
    list_graph_t *graph;
	char buf[256];

    // stupid fgets workaround for lambda
    fgets(buf, 256, stdin);
    sscanf(buf, "%d %d\n", &n, &m);

    graph = lg_create(n);

    // Read graph edges
    for (int i = 0; i < m; ++i)
    {
        fgets(buf, 256, stdin);
        sscanf(buf,"%d %d\n", &src, &dest);
        lg_add_edge(graph, src, dest);
    }

    // Read src & dest for min path
    fgets(buf, 256, stdin);
    sscanf(buf,"%d %d\n", &src, &dest);

    // Find & print min path
    print_min_path(graph, src, dest);

    lg_free(graph);

    return 0;
}