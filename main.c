#include <stdio.h>
#include <pthread.h>
#include "slist.h"

void* producer(void* arg) {
    list_t* lst = (list_t*)arg;
    char* str_all[] = { "one", "two", "three", "four", "five" };
    int count = sizeof(str_all) / sizeof(str_all[0]);
    for (int i = 0; i < count; i++) {
        l_add(lst, str_all[i]);
        printf("string nr %s - producer\n", str_all[i]);
        l_print(lst);  
    }

    return NULL;
}


void* consumer(void* arg){
    list_t* lst =(list_t*)arg;
    for(int i = 0; i < 5; i++){
        char* str = l_get(lst);
        if(str){
            printf("string nr: %s - consumer\n", str);
        }
        l_print(lst);
    }
    return NULL;
}

int main(){

    list_t* list1 = l_init(5);
    list_t* list2 = l_init(5);
    pthread_t prod_thread1, prod_thread2, cons_thread1, cons_thread2;

    pthread_create(&prod_thread1, NULL, producer, list1);
    pthread_create(&cons_thread1, NULL, consumer, list1);

    pthread_create(&prod_thread2, NULL, producer, list2);
    pthread_create(&cons_thread2, NULL, consumer, list2);

    pthread_join(prod_thread1, NULL);
    pthread_join(cons_thread1, NULL);
    pthread_join(prod_thread2, NULL);
    pthread_join(cons_thread2, NULL);


    l_destroy(list1);
    l_destroy(list2);

    return 0;
}
