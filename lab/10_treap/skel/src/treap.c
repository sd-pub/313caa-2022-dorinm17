#include "treap.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_PRIORITY 1000

// Creeaza structura Treap-ului
Treap_tree*
treap_create(int (*cmp)(void*, void*))
{
    srand(time(NULL));
    Treap_tree* tree = malloc(sizeof(Treap_tree));
    tree->compar = cmp;
    if (tree == NULL) {
        return NULL;
    }
    tree->root = NULL;
    return tree;
}
void
node_free(Node** node)
{

    if (!(*node)) {
        return;
    }
    node_free(&(*node)->left);
    node_free(&(*node)->right);

    free((*node)->data);
    free(*node);
}

void
treap_free(Treap_tree* tree)
{
    if (tree->root != NULL) {
        node_free(&tree->root);
    }
    free(tree);
}
/* Creeaza un nod
 * @param1: Valoarea ce trebuie pusa in nod.
 * @param2: Numarul de octeti pe care scrie valoarea.
 */
Node*
node_create(void* value, int data_size)
{
    Node* node = malloc(sizeof(Node));
    if (node == NULL) {
        return NULL;
    }
    node->data = malloc(data_size);

    // Copiere octet cu octet din value, in nodul curent.
    for (int i = 0; i < data_size; ++i) {
        *(char*)(node->data + i) = *(char*)(value + i);
    }

    // Nodurile frunze au inaltimea 0.
    node->priority = rand() % MAX_PRIORITY;
    node->left = NULL;
    node->right = NULL;

    return node;
}

int
max(int a, int b)
{
    if (a > b) {
        return a;
    }
    return b;
}
// Nodurile NULL au prioritatea -1 pentru a pastra proprietatea de max-heap.
int
priority(Node* node)
{
    if (node == NULL) {
        return -1;
    }
    return node->priority;
}
/* Rotire dreapta
 * Pentru a nu fi nevoie sa mentinem pointer catre nodul parinte,
 * se vor folosi pointeri la noduri
 *
 *      node  			    lson
 *     /    \			   /    \
 *   lson    y    --->    x    node
 *   /  \     		           /   \
 *  x   lrson                lrson  y
 */
void
rotate_right(Node** node)
{
    // TODO: Completati rotire dreapta
}
/* Rotire stanga
 * Pentru a nu fi nevoie sa mentinem pointer catre nodul parinte,
 * se vor folosi pointeri la noduri
 *
 *     node  			    rson
 *    /    \			   /    \
 *   x     rson    --->  node    y
 *         /   \		 /   \
 *       rlson  y       x   rlson
 */
void
rotate_left(Node** node)
{
    // TODO: Completati rotire stanga.
}
/* Inserare in Treap
 *
 * @param1: Nodul radacina al subarborelui din parcurgerea recursiva.
 * @param2: Valoare de adaugat in Treap.
 * @param3: Numarul de octeti pe care se scrie valoarea.
 * @param4: Functia de comparare pentru datele din Treap.
 */
void
treap_insert(Node** node,
             void* value,
             int data_size,
             int (*compar)(void*, void*))
{
    // TODO: Inserati recursiv in arbore

    // TODO: Reechilibrare arbore
}

/* Stergere din Treap
 *
 * @param1: Nodul radacina al subarborelui din parcurgerea recursiva.
 * @param2: Valoare de adaugat in Treap.
 * @param3: Numarul de octeti pe care se scrie valoarea.
 * @param4: Functia de comparare pentru datele din Treap.
 */
void
treap_delete(Node** node,
             void* value,
             int data_size,
             int (*compar)(void*, void*))
{
    // TODO: Stergeti recursiv din arbore
}
void*
get_key(Node* node, void* value, int data_size, int (*compar)(void*, void*))
{
    // TODO: Cautarea unei valori in arbore
}

/* Verifica daca un arbore respecta proprietatile unui treap
 *
 * @param1: Nodul curent in parcurgerea recursiva.
 * @param2: Functia de comparare a datelor din fiecare nod.
 * @return: Daca arborele e Treap, vom returna numarul de noduri al arborelui,
 * 			altfel, vom returna o valoare negativa.
 */
int
check_treap(Node* node, int (*compar)(void*, void*))
{
    if (node == NULL) {
        return 0;
    } else if (node->left == NULL && node->right == NULL) {
        return 1;
    } else if (node->left == NULL) {
        if (priority(node) >= priority(node->right) &&
            compar(node->data, node->right->data) <= 0) {
            return 1 + check_treap(node->right, compar);
        }
        return INT_MIN;
    } else if (node->right == NULL) {
        if (priority(node) >= priority(node->left) &&
            compar(node->data, node->left->data) >= 0) {
            return 1 + check_treap(node->left, compar);
        }
        printf("%d %d %d\n",
               priority(node),
               priority(node->left),
               priority(node->right));
        return INT_MIN;
    } else {
        if (priority(node) >= priority(node->left) &&
            priority(node) >= priority(node->right) &&
            compar(node->data, node->left->data) >= 0 &&
            compar(node->data, node->right->data) <= 0) {
            return 1 + check_treap(node->left, compar) +
                   check_treap(node->right, compar);
        }
        printf("%d %d %d\n",
               priority(node),
               priority(node->left),
               priority(node->right));
        return INT_MIN;
    }
}
