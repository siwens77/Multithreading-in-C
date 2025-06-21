#ifndef LCS_LIST_H
#define LCS_LIST_H
#include <pthread.h>

typedef struct Node {
    char* str;
    struct Node* prev;
    struct Node* next;
} Node;

struct list_t {
    Node* head;
    Node* tail;
    int c;
    int size;

    pthread_mutex_t lock;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
}; 

typedef struct list_t list_t;

list_t* l_init(int c);

void l_destroy(list_t* lst);

void l_add(list_t* lst, char* str);

char* l_get(list_t* lst);

char* l_pop(list_t* lst);

int l_remove(list_t* lst, char* str);

int l_count(list_t* lst);

void l_setcapacity(list_t* lst, int c);

void l_remove_duplicates(list_t* lst);

void l_join(list_t* lst, list_t* lst2);

void l_clear(list_t* lst);

void l_print(list_t* lst);

#endif