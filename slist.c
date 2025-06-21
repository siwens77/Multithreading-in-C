#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"
#include <pthread.h>


list_t* l_init(int c){
    list_t* lst = malloc(sizeof(list_t));
    if (!lst){
    printf("LIST NOT CREATED(error in l_init)\n");
    return NULL;
    } 
    lst->head = NULL;
    lst->tail = NULL;
    lst->c = c;
    lst->size = 0;
    pthread_mutex_init(&lst->lock, NULL);
    pthread_cond_init(&lst->not_full, NULL);
    pthread_cond_init(&lst->not_empty, NULL);
    return lst;//creating list
} 

void l_destroy(list_t *lst){

    pthread_mutex_lock(&lst->lock);
    if (lst->size == 0){
        pthread_mutex_unlock(&lst->lock);
        pthread_mutex_destroy(&lst->lock);
        pthread_cond_destroy(&lst->not_full);
        pthread_cond_destroy(&lst->not_empty);
        free(lst);
    } 
    else {
        while (lst->head){
            Node* curr = lst->head;
            if (curr != lst->tail){
                lst->head = lst->head->next;
                lst->head->prev = NULL;
            } else {
                lst->head = NULL;
                lst->tail = NULL;
            }
            free(curr);
        }
        pthread_mutex_unlock(&lst->lock);
        pthread_mutex_destroy(&lst->lock);
        pthread_cond_destroy(&lst->not_full);
        pthread_cond_destroy(&lst->not_empty);
        free(lst);
    }
}

void l_add(list_t *lst, char *str){
    
    pthread_mutex_lock(&lst->lock);
    while(lst->size >=lst->c){
        pthread_cond_wait(&lst-> not_full,&lst-> lock);
    }

    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->str = str;

    if (lst->size == 0){
        lst->head = new_node;
        lst->tail = new_node;
        new_node->prev=NULL;
        new_node->next=NULL;
    }

    else if(strcmp(str, lst->tail->str) >= 0){
        new_node->next= NULL;
        new_node->prev = lst->tail;
        lst->tail->next = new_node;
        lst->tail = new_node;
    }
    else if(strcmp(str, lst->head->str) <= 0){
        new_node->prev = NULL;
        new_node->next = lst->head;
        lst->head->prev = new_node;
        lst->head = new_node;
    }
    else{
        Node* curr = lst->head->next; 
        while (curr){
            if (strcmp(str, curr->str) <= 0){
                new_node->next = curr;
                curr->prev->next = new_node;
                new_node->prev = curr->prev;
                curr->prev = new_node;
                break;
            }
            curr = curr->next;
        }
    }
    lst->size++;

    pthread_cond_signal(&lst->not_empty);
    pthread_mutex_unlock(&lst->lock);


}

char* l_get(list_t *lst){

    pthread_mutex_lock(&lst->lock);
    while(lst->size == 0){
        pthread_cond_wait(&lst-> not_empty,&lst-> lock);
    }

    Node *temp = lst->head;
    if(lst->head != lst->tail){
        lst->head = lst->head->next;
        if (lst->head){
            lst->head->prev = NULL;
        }
    }else{
        lst->head = NULL;
        lst->tail = NULL;
    }
    char *str = temp->str;
    free(temp);
    lst->size--;

    pthread_cond_signal(&lst-> not_full);
    pthread_mutex_unlock(&lst->lock);

    return str;
}

char* l_pop(list_t *lst){

    pthread_mutex_lock(&lst->lock);
    while(lst->size == 0){
        pthread_cond_wait(&lst-> not_empty,&lst-> lock);
    }

    Node *temp = lst->tail;
    if (lst->head != lst->tail){
        lst->tail = lst->tail->prev;
        if (lst->tail){
            lst->tail->next = NULL;
        }
    }else{
        lst->head = NULL;
        lst->tail = NULL;
    }
    char *str = temp->str;
    free(temp);
    lst->size--;

    pthread_cond_signal(&lst-> not_full);
    pthread_mutex_unlock(&lst->lock);

    return str;
}

int l_remove(list_t *lst, char *str){
    pthread_mutex_lock(&lst->lock);
    if (lst->size == 0){
        printf("LIST IS EMPTY(error in l_remove)\n");
        pthread_mutex_unlock(&lst->lock);
        return 1;
    }
    if(lst->size == 1){
        if(lst->head->str == str){
            Node* temp = lst->head;
            lst->head = NULL;
            lst->tail = NULL;
            free(temp);
            lst->size--;
            pthread_mutex_unlock(&lst->lock);
            return 0;
        }else{
            printf("STRING NOT FOUND! RETURN 1\n");
            pthread_mutex_unlock(&lst->lock);
            return 1;          
        }
    }

    if (lst->head->str == str){
        Node* temp = lst->head;
        lst->head = lst->head->next;
        lst->head->prev = NULL;
        free(temp);
        lst->size--;
        pthread_mutex_unlock(&lst->lock);
        return 0;   
    }
    else if (lst->tail->str == str){
        Node* temp = lst->tail;
        lst->tail = lst->tail->prev;
        lst->tail->next = NULL;
        free(temp);
        lst->size--;
        pthread_mutex_unlock(&lst->lock);
        return 0;   
    }else{
        Node* curr = lst->head;
        while (curr){
            if (curr->str == str){
                curr->prev->next = curr->next;
                curr->next->prev = curr->prev;
                free(curr);
                lst->size--;
                pthread_mutex_unlock(&lst->lock);
                return 0;
            }
            curr = curr->next;
        }
    }
    pthread_mutex_unlock(&lst->lock);
    return 1;
}

int l_count(list_t *lst){
    pthread_mutex_lock(&lst->lock);
    int count;
    count = lst->size;
    printf("NUMBER OF STRINGS IN LIST: %d\n", count);
    pthread_mutex_unlock(&lst->lock);
    return count;
}

void l_setcapacity(list_t *lst, int c){
    pthread_mutex_lock(&lst->lock);
    lst->c = c;
    if (lst->size >= lst->c){
        printf("CHOSEN CAPACITY IS SMALLER THAN CURRENT LIST SIZE! \n");
        printf("NO ELEMENTS CAN BE ADDED UNTIL SIZE OF LIST < NEW CAPACITY\n");
    }
    pthread_mutex_unlock(&lst->lock);
}

void l_remove_duplicates(list_t *lst){
    pthread_mutex_lock(&lst->lock);

    Node* curr = lst->head;
    while (curr != lst->tail){
        if (strcmp(curr->str, curr->next->str) == 0){
            if(curr->next != lst->tail){
                Node* temp = curr->next;
                curr->next = curr->next->next;
                curr->next->prev = curr;
                free(temp);
                lst->size--;
            }else{
                Node* temp = curr->next;
                curr->next = curr->next->next;
                lst->tail = curr;
                free(temp);
            }
        }else{
            curr = curr->next;
        }
    }
    pthread_mutex_unlock(&lst->lock);
}

void l_join(list_t *lst1, list_t *lst2){
    pthread_mutex_lock(&lst1->lock);
    pthread_mutex_lock(&lst2->lock);
    int temp_c = lst1->c;
    l_setcapacity(lst1, lst1->c + lst2->size);
    while(lst2->head){
        l_add(lst1, lst2->head->str);
        if(lst2->head != lst2->tail){
            Node* temp= lst2->head;
            lst2->head->next->prev = NULL;
            lst2->head = lst2->head->next;
            free(temp);
        }else{
            Node *temp = lst2->head;
            lst2->head = NULL;
            lst2->tail = NULL;
            free(temp);
        }
        lst2->size--;
    }
    l_setcapacity(lst1, temp_c);
    pthread_mutex_unlock(&lst1->lock);
    pthread_mutex_unlock(&lst2->lock);
}

void l_clear(list_t *lst){
    pthread_mutex_lock(&lst->lock);
    if(lst->size == 0){
        pass;
    }else{
        while(lst->head){ 
            Node* curr = lst->head;
            if(curr!=lst->tail){
                lst->head = lst->head->next;
                lst->head->prev = NULL;
            }else{
                lst->head = NULL;
                lst->tail = NULL;
            }
            free(curr);
            lst->size--;
            printf("%d\n", lst->size);
        }
    }
    pthread_mutex_unlock(&lst->lock);
}

void l_print(list_t *lst){
    pthread_mutex_lock(&lst->lock);
    if (lst->size == 0){
        printf("SIZE IS ZERO! (error in l_print)\n");
        pthread_mutex_unlock(&lst->lock);
        return;
    }else{
        printf("NULL-");
        Node* curr = lst->head;
        while(curr){
            printf("%s-", curr->str);
            curr = curr->next;
        }
        printf("NULL\n");
    }
    pthread_mutex_unlock(&lst->lock);
}
