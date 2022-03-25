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
#define HMAX 10

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
    linked_list_t *ll;

    ll = malloc(sizeof(*ll));
    DIE(ll == NULL, "linked_list malloc");

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
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
    DIE(new_node == NULL, "new_node malloc");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "new_node->data malloc");
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

typedef struct info info;
struct info {
	void *key;
	void *value;
};

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets; /* Array de liste simplu-inlantuite. */
	/* Nr. total de noduri existente curent in toate bucket-urile. */
	unsigned int size;
	unsigned int hmax; /* Nr. de bucket-uri. */
	/* (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor. */
	unsigned int (*hash_function)(void*);
	/* (Pointer la) Functie pentru a compara doua chei. */
	int (*compare_function)(void*, void*);
};

/*
 * Functii de comparare a cheilor:
 */
int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functii de hashing:
 */
unsigned int
hash_function_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int
hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t *ht;

    ht = malloc(sizeof(*ht));
    DIE(ht == NULL, "hashtable malloc");
    ht->buckets = malloc(hmax * sizeof(linked_list_t));
    DIE(ht->buckets == NULL, "hashtable buckets malloc");

    ht->size = 0;
    ht->hmax = hmax;
    ht->hash_function = hash_function;
    ht->compare_function = compare_function;

    for (unsigned int i = 0; i < hmax; i++)
        ht->buckets[i] = ll_create(sizeof(info));

    return ht;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable folosind functia put
 * 0, altfel.
 */
int
ht_has_key(hashtable_t *ht, void *key)
{
    if (!ht)
        return 0;
    if (!ht->buckets)
        return 0;

    unsigned int index = ht->hash_function(key) % ht->hmax;
    if (!ht->buckets[index])
        return 0;

    ll_node_t* curr = ht->buckets[index]->head;

    while (curr) {
        if (ht->compare_function(((info *)curr->data)->key, key) == 0)
            return 1;
        curr = curr->next;
    }

	return 0;
}

void *
ht_get(hashtable_t *ht, void *key)
{
	if (!ht)
        return NULL;
    if (!ht->buckets)
        return NULL;

    unsigned int index = ht->hash_function(key) % ht->hmax;
    if (!ht->buckets[index])
        return NULL;

    ll_node_t* curr = ht->buckets[index]->head;

    while (curr) {
        if (ht->compare_function(((info *)curr->data)->key, key) == 0)
            return ((info *)curr->data)->value;
        curr = curr->next;
    }

	return NULL;
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune tipul ei), in momentul in care
 * se creeaza o noua intrare in hashtable (in cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie
 * a valorii la care pointeaza key si adresa acestei copii trebuie salvata in structura info asociata intrarii din ht.
 * Pentru a sti cati octeti trebuie alocati si copiati, folositi parametrul key_size.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel put(ht, key_actual, value_actual),
 * valoarea la care pointeaza key_actual poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia unei intrari din hashtable.
 * Nu ne dorim acest lucru, fiindca exista riscul sa ajungem in situatia in care nu mai stim la ce cheie este
 * inregistrata o anumita valoare.
 */
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	if (!ht)
        return;
    if (!ht->buckets)
        return;

    unsigned int index = ht->hash_function(key) % ht->hmax;
    if (!ht->buckets[index])
        return;

    ll_node_t* curr = ht->buckets[index]->head;

    while (curr) {
        if (ht->compare_function(((info *)curr->data)->key, key) == 0) {
            memcpy(((info *)curr->data)->value, value, value_size);
            return;
        }
        curr = curr->next;
    }

    info new_info;

    new_info.key = malloc(key_size);
    DIE(new_info.key == NULL, "new_info mallloc");
    new_info.value = malloc(value_size);
    DIE(new_info.value == NULL, "new_info mallloc");

    memcpy(new_info.key, key, key_size);
    memcpy(new_info.value, value, value_size);

    ll_add_nth_node(ht->buckets[index], ht->buckets[index]->size, &new_info);

    ht->size++;
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o intrare din hashtable (adica memoria
 * pentru copia lui key --vezi observatia de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void
ht_remove_entry(hashtable_t *ht, void *key)
{
	if (!ht)
        return;
    if (!ht->buckets)
        return;

    unsigned int index = ht->hash_function(key) % ht->hmax;
    if (!ht->buckets[index])
        return;

    ll_node_t *curr = ht->buckets[index]->head;
    unsigned int ct = 0;

    while (curr) {
        if (ht->compare_function(((info *)curr->data)->key, key) == 0) {
            ll_node_t *removed = ll_remove_nth_node(ht->buckets[index], ct);
            free(((info *)removed->data)->key);
            free(((info *)removed->data)->value);
            free(removed->data);
            free(removed);
            break;
        }
        curr = curr->next;
        ct++;
    }

    ht->size--;
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable, dupa care elibereaza si memoria folosita
 * pentru a stoca structura hashtable.
 */
void
ht_free(hashtable_t *ht)
{	
    for (unsigned int i = 0; i < ht->hmax; i++) {
        ll_node_t *curr = ht->buckets[i]->head;
        for (unsigned int j = 0; j < ht->buckets[i]->size; j++) {
            free(((info *)curr->data)->key);
            free(((info *)curr->data)->value);
            curr = curr->next;
        }
        ll_free(&ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}

struct FriendInfo {
	char name[MAX_STRING_SIZE];
	char faculty[MAX_STRING_SIZE];
	char fav_food[MAX_STRING_SIZE];
	int age;
};

static void
print_friend(struct FriendInfo *f_info)
{
	if (f_info == NULL) {
		printf("Friend doesn't exist!\n");
		return;
	}
	printf("Name: %s | ", f_info->name);
	printf("Age: %d | ", f_info->age);
	printf("Faculty: %s | ", f_info->faculty);
	printf("Favorite food: %s\n", f_info->fav_food);
}

int
main()
{
	hashtable_t *ht_friends = NULL;
	struct FriendInfo info_f;
	struct FriendInfo *p_infoFriend;

	while (1) {
		char command[MAX_STRING_SIZE];

		scanf("%s", command);
		if (strncmp(command, "create_ht", 9) == 0) {
			ht_friends = ht_create(HMAX, hash_function_string,
				compare_function_strings);

		} else if (strncmp(command, "add_friend", 10) == 0) {
			scanf("%s %d %s %s", info_f.name, &info_f.age,
				info_f.faculty, info_f.fav_food);

			if (!ht_friends) {
				printf("Create hashtable first.\n");
			} else {
				ht_put(ht_friends, info_f.name, strlen(info_f.name) + 1,
					&info_f, sizeof(info_f));
			}
			
		} else if (strncmp(command, "print_friend", 12) == 0) {
			char name[MAX_STRING_SIZE];

			scanf("%s", name);

			if (!ht_friends) {
				printf("Create hashtable first.\n");
			} else {
				p_infoFriend = ht_get(ht_friends, name);
				print_friend(p_infoFriend);
			}

		} else if (strncmp(command, "check_friend", 12) == 0) {
			char name[MAX_STRING_SIZE];

			scanf("%s", name);
			if (!ht_friends) {
				printf("Create hashtable first.\n");
			} else if(ht_has_key(ht_friends, name)) {
				printf("Friend %s exists.\n", name);
			} else {
				printf("Friend %s doesn't exist.\n", name);
			}

		} else if (strncmp(command, "remove_friend", 13) == 0) {
			char name[MAX_STRING_SIZE];

			scanf("%s", name);
			if (!ht_friends) {
				printf("Create hashtable first.\n");
			} else {
				ht_remove_entry(ht_friends, name);
			}

		} else if (strncmp(command, "free", 4) == 0) {
			if (!ht_friends) {
				printf("Create hashtable first.\n");
			} else {
				ht_free(ht_friends);
			}
			break;
		}
	}

	return 0;
}
