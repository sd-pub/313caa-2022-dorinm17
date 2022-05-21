/*
    SD 2022 - Trie
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* useful macro for handling error codes */
#define DIE(assertion, call_description)  \
	do {								  \
		if (assertion) {				  \
			fprintf(stderr, "(%s, %d): ", \
					__FILE__, __LINE__);  \
			perror(call_description);	  \
			exit(errno);				  \
		}							      \
	} while (0)                           \

#define ALPHABET_SIZE 26
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"

typedef struct trie_node_t trie_node_t;
struct trie_node_t {
    /* Value associated with key (set if end_of_word = 1) */
    void *value;

    /* 1 if current node marks the end of a word, 0 otherwise */
    int end_of_word;

    trie_node_t **children;
    int n_children;
};

typedef struct trie_t trie_t;
struct trie_t {
    trie_node_t *root;

    /* Number of keys */
    int size;

    /* Generic Data Structure */
    int data_size;

    /* Trie-Specific, alphabet properties */
    int alphabet_size;
    char *alphabet;

    /* Callback to free value associated with key, should be called when freeing */
    void (*free_value_cb)(void *);

    /* Optional - number of nodes, useful to test correctness */
    int nNodes;
};

trie_node_t *trie_create_node(trie_t *trie)
{
    trie_node_t *node;
    node = malloc(sizeof(*node));
    DIE(!node, "node malloc failed\n");

    node->children = malloc(trie->alphabet_size * sizeof(trie_node_t *));
    DIE(!node->children, "node->children malloc failed\n");
    for (int i = 0; i < trie->alphabet_size; i++)
        node->children[i] = NULL;

    node->n_children = 0;
    node->end_of_word = 0;

    trie->nNodes++;

    return node;
}

trie_t *trie_create(int data_size, int alphabet_size, char *alphabet, void (*free_value_cb)(void *))
{
    trie_t *trie;
    trie = malloc(sizeof(*trie));
    DIE(!trie, "trie malloc failed\n");

    trie->size = 1;
    trie->nNodes = 0;
    trie->data_size = data_size;
    trie->free_value_cb = free_value_cb;
    trie->alphabet_size = alphabet_size;

    trie->alphabet = malloc(alphabet_size * sizeof(char));
    DIE(!trie->alphabet, "trie->alphabet malloc failed\n");
    memcpy(trie->alphabet, alphabet, alphabet_size * sizeof(char));

    trie->root = trie_create_node(trie);
    trie->root->end_of_word = 1;

    int aux = -1;
    trie->root->value = malloc(trie->data_size);
    DIE(!trie->root->value, "tree->root->value malloc failed\n");
    memcpy(trie->root->value, &aux, trie->data_size);

    return trie;
}

void trie_insert(trie_t *trie, char *key, void *value)
{
    trie_node_t *curr = trie->root, *new_node;
    int index;

    while (strlen(key)) {
        index = *key - *trie->alphabet;

        if (!curr->children[index]) {
            new_node = trie_create_node(trie);
            curr->children[index] = new_node;
            curr->n_children++;
        }

        curr = curr->children[index];
        key++;
    }

    if (!curr->end_of_word) {
        curr->value = malloc(trie->data_size);
        DIE(!curr->value, "curr->valiue malloc failed\n");
        trie->size++;
    }

    memcpy(curr->value, value, trie->data_size);
    curr->end_of_word = 1;
}

void *trie_search(trie_t *trie, char *key)
{
    trie_node_t *curr = trie->root;
    int index;

    while (strlen(key)) {
        index = *key - *trie->alphabet;

        if (!curr->children[index])
            return NULL;

        curr = curr->children[index];
        key++;
    }

    if (!curr->end_of_word)
        return NULL;

    return curr->value;
}

int trie_node_remove(trie_node_t *curr, trie_t *trie, char *key)
{
    if (!*key) {
        if (curr->end_of_word) {
            trie->free_value_cb(curr->value);
            curr->end_of_word = 0;

            return !curr->n_children;
        }

        return 0;
    }

    int index = *key - *trie->alphabet;

    if (curr->children[index] &&
        trie_node_remove(curr->children[index], trie, key + 1)) {
        free(curr->children[index]->children);
        free(curr->children[index]);
        curr->children[index] = NULL;

        trie->nNodes--;
        curr->n_children--;

        return !curr->n_children && !curr->end_of_word;
    }

    return 0;
}

void trie_remove(trie_t *trie, char *key)
{
    trie_node_remove(trie->root, trie, key);
    trie->size--;
}

void trie_node_free(trie_node_t **curr, trie_t *trie)
{
    for (int i = 0; i < trie->alphabet_size; i++)
        if ((*curr)->children[i])
            trie_node_free(&((*curr)->children[i]), trie);

    if ((*curr)->end_of_word)
        trie->free_value_cb((*curr)->value);

    free((*curr)->children);
    free(*curr);
}

void trie_free(trie_t **pTrie)
{
    trie_node_free(&((*pTrie)->root), *pTrie);
    free((*pTrie)->alphabet);
    free(*pTrie);
}

/* Needed for Lambda tests, ignore :) */
void cleanup_example_string(char *str)
{
    int len = strlen(str);

    if(str[len-2] == '\\') {
        str[len-2] = '\0';
    }
}

int main(void)
{
    int n, value;
    char alphabet[] = ALPHABET;
    char buf[256], key[256], op;

    trie_t* trie = trie_create(sizeof(int), ALPHABET_SIZE, alphabet, free);

    fgets(buf, 256, stdin);
	sscanf(buf, "%d\n", &n);

    for (int i = 0; i < n; ++i) {
        fgets(buf, 256, stdin);
	    sscanf(buf, "%c", &op);

        if (op == 'i') {
	        sscanf(buf, "%c %s %d\n", &op, key, &value);
            trie_insert(trie, key, &value);
        } else if(op == 'r') {
	        sscanf(buf, "%c %s\n", &op, key);
            cleanup_example_string(key);
            printf("nNodes before removing %s: %d\n", key, trie->nNodes);
            trie_remove(trie, key);
            printf("nNodes after removing %s: %d\n", key, trie->nNodes);
        } else if(op == 's') {
            sscanf(buf, "%c %s\n", &op, key);
            cleanup_example_string(key);

            if (key[0] == '_') {
                key[0] = '\0';
            }

            int *found = trie_search(trie, key);
            printf("%s: ", key[0] == '\0' ? "_" : key);

            if (found) {
                printf("%d\n", *found);
            } else {
                printf("not found\n");
            }
        }
    }

    trie_free(&trie);

    return 0;
}
