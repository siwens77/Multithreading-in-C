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

void l_destroy(list_t *lst){ //DONE

    pthread_mutex_lock(&lst->lock);
    if (lst->size == 0){
        pthread_mutex_unlock(&lst->lock);
        pthread_mutex_destroy(&lst->lock);
        pthread_cond_destroy(&lst->not_full);
        pthread_cond_destroy(&lst->not_empty);
        free(lst);
    } //case if list is empty
    else {
        while (lst->head){
            Node* curr = lst->head;
            if (curr != lst->tail){ //until reaching end
                lst->head = lst->head->next; //moving forward
                lst->head->prev = NULL; //deleting previous
            } else {
                lst->head = NULL;
                lst->tail = NULL;
            }
            free(curr); //freeing pointer and head and tail
        }
        pthread_mutex_unlock(&lst->lock);
        pthread_mutex_destroy(&lst->lock);
        pthread_cond_destroy(&lst->not_full);
        pthread_cond_destroy(&lst->not_empty);
        free(lst);
    }
}

void l_add(list_t *lst, char *str){//DONE
    
    pthread_mutex_lock(&lst->lock);//lock mutex
    while(lst->size >=lst->c){//cannot add, as long as it leaks
        pthread_cond_wait(&lst-> not_full,&lst-> lock);//wait until you can work
    }

    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->str = str;//new node with string

    if (lst->size == 0){
        lst->head = new_node;
        lst->tail = new_node;
        new_node->prev=NULL;
        new_node->next=NULL;
    }//creating node if empty list

    else if(strcmp(str, lst->tail->str) >= 0){//checking if string new tail
        new_node->next= NULL;//clearing next in new tail
        new_node->prev = lst->tail;//A -> B
        lst->tail->next = new_node;//A <-> B
        lst->tail = new_node;// tail == A
    }
    else if(strcmp(str, lst->head->str) <= 0){//checking if string is new head
        new_node->prev = NULL;//clearing previous in new for safety
        new_node->next = lst->head;//A -> B
        lst->head->prev = new_node;//A <-> B
        lst->head = new_node;//head == A
    }
    else{
        Node* curr = lst->head->next; //string between head and tail
        while (curr){
            if (strcmp(str, curr->str) <= 0){//place found
                new_node->next = curr;//new->curr
                curr->prev->next = new_node;//before->new
                new_node->prev = curr->prev;//before<->new
                curr->prev = new_node;//new<->curr
                break;
            }
            curr = curr->next;//moving forward
        }
    }
    lst->size++;//increasing size of the list

    pthread_cond_signal(&lst->not_empty);
    pthread_mutex_unlock(&lst->lock);


}

char* l_get(list_t *lst){//DONE

    pthread_mutex_lock(&lst->lock);
    while(lst->size == 0){
        pthread_cond_wait(&lst-> not_empty,&lst-> lock);//wait until you can work
    }

    Node *temp = lst->head;
    if(lst->head != lst->tail){//case if more than 1
        lst->head = lst->head->next;//new head in list
        if (lst->head){
            lst->head->prev = NULL;
        }
    }else{//just clear the list
        lst->head = NULL;
        lst->tail = NULL;
    }
    char *str = temp->str;
    free(temp);//freeing
    lst->size--;//decreasing int size

    pthread_cond_signal(&lst-> not_full);
    pthread_mutex_unlock(&lst->lock);

    return str;
}

char* l_pop(list_t *lst){//DONE

    pthread_mutex_lock(&lst->lock);
    while(lst->size == 0){
        pthread_cond_wait(&lst-> not_empty,&lst-> lock);//wait until you can work
    }

    Node *temp = lst->tail;
    if (lst->head != lst->tail){//if more than 1 element
        lst->tail = lst->tail->prev;//moving tail back
        if (lst->tail){
            lst->tail->next = NULL;//tail->NULL
        }
    }else{//just clearing the list
        lst->head = NULL;
        lst->tail = NULL;
    }
    char *str = temp->str;
    free(temp);//freeing temporary
    lst->size--;

    pthread_cond_signal(&lst-> not_full);
    pthread_mutex_unlock(&lst->lock);

    return str;
}

int l_remove(list_t *lst, char *str){//DONE
    pthread_mutex_lock(&lst->lock);
    if (lst->size == 0){
        printf("LIST IS EMPTY(error in l_remove)\n");
        pthread_mutex_unlock(&lst->lock);
        return 1;
    }
    if(lst->size == 1){//only one element
        if(lst->head->str == str){//comparing pointers
            Node* temp = lst->head;
            lst->head = NULL;
            lst->tail = NULL;
            free(temp);
            lst->size--;
            pthread_mutex_unlock(&lst->lock);
            return 0;//worked!
        }else{
            printf("STRING NOT FOUND! RETURNING 1\n");
            pthread_mutex_unlock(&lst->lock);
            return 1;          
        }
    }

    if (lst->head->str == str){//if head is an answer
        Node* temp = lst->head;
        lst->head = lst->head->next;
        lst->head->prev = NULL;//removing head
        free(temp);
        lst->size--;//decreasing size of list
        printf("REMOVED HEAD\n");
        pthread_mutex_unlock(&lst->lock);
        return 0;   
    }
    else if (lst->tail->str == str){//if tail is an answer
        Node* temp = lst->tail;
        lst->tail = lst->tail->prev;
        lst->tail->next = NULL;//removing last
        free(temp);
        lst->size--;
        printf("l_remove found at tail\n");
        pthread_mutex_unlock(&lst->lock);
        return 0;   
    }else{//normal case
        Node* curr = lst->head;
        while (curr){
            if (curr->str == str){//if found
                curr->prev->next = curr->next;
                curr->next->prev = curr->prev;//deleting
                free(curr);
                lst->size--;//decreasing
                printf("l_remove found\n");
                pthread_mutex_unlock(&lst->lock);
                return 0;
            }
            curr = curr->next;
        }
    }
    printf("NOT FOUND?\n"); 
    pthread_mutex_unlock(&lst->lock);
    return 1;
}

int l_count(list_t *lst){//DONE
    pthread_mutex_lock(&lst->lock);//locking list
    int count;
    count = lst->size;
    printf("NUMBER OF STRINGS IN LIST %d\n", count);//must work no edge cases
    pthread_mutex_unlock(&lst->lock);//unlocking list
    return count;//returning size
}

void l_setcapacity(list_t *lst, int c){//DONE
    pthread_mutex_lock(&lst->lock);
    lst->c = c;
    if (lst->size >= lst->c){
        printf("CHOSEN CAPACITY IS SMALLER THAN CURRENT LIST SIZE! \n");
        printf("NO ELEMENTS CAN BE ADDED UNTIL SIZE OF LIST < NEW CAPACITY\n");
    }
    pthread_mutex_unlock(&lst->lock);
}

void l_remove_duplicates(list_t *lst){//DONE
    pthread_mutex_lock(&lst->lock);
    if (lst->size < 2){
        printf("LIST SMALLER THAN 2, NO DUPLICATES \n");
    }

    Node* curr = lst->head;
    while (curr != lst->tail){//until too far
        if (strcmp(curr->str, curr->next->str) == 0){//if the same
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
            curr = curr->next;//moving forward only if not duplicate
        }
    }
    pthread_mutex_unlock(&lst->lock);
}

void l_join(list_t *lst1, list_t *lst2){//DONE
    pthread_mutex_lock(&lst1->lock);
    pthread_mutex_lock(&lst2->lock);
    int temp_c = lst1->c;
    l_setcapacity(lst1, lst1->c + lst2->size);//new capacity
    while(lst2->head){
        l_add(lst1, lst2->head->str);//adding head 
        if(lst2->head != lst2->tail){//if not last one
            Node* temp= lst2->head;
            lst2->head->next->prev = NULL;
            lst2->head = lst2->head->next;
            free(temp);
        }else{//ending if over
            Node *temp = lst2->head;
            lst2->head = NULL;
            lst2->tail = NULL;
            free(temp);
        }
        lst2->size--;
    }
    l_setcapacity(lst1, temp_c);//updating capacity
    pthread_mutex_unlock(&lst1->lock);
    pthread_mutex_unlock(&lst2->lock);
}

void l_clear(list_t *lst){//DONE
    pthread_mutex_lock(&lst->lock);
    if(lst->size == 0){
        printf("LIST EMPTY ALREADY\n");//case if list empty
    }else{
        while(lst->head){ 
            Node* curr = lst->head;
            if(curr!=lst->tail){//if not the end
                lst->head = lst->head->next;//move forward
                lst->head->prev = NULL;
            }else{//if end
                lst->head = NULL;//clear
                lst->tail = NULL;
            }
            free(curr);
            lst->size--;//decrease size
            printf("%d\n", lst->size);
        }
    }
    pthread_mutex_unlock(&lst->lock);
}

void l_print(list_t *lst){//DONE
    pthread_mutex_lock(&lst->lock);
    if (lst->size == 0){
        printf("SIZE IS ZERO! (error in l_print)\n");
        pthread_mutex_unlock(&lst->lock);
        return;
    }else{
        printf("NULL-");//before head
        Node* curr = lst->head;
        while(curr){//move forward
            printf("%s-", curr->str);//print next and next...
            curr = curr->next;
        }
        printf("NULL\n");//after tail
    }
    pthread_mutex_unlock(&lst->lock);
}