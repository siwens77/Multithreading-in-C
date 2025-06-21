# Thread-Safe Sorted List in C with POSIX Threads
Project made for University course - Operating systems and Concurrent Programming.<br> This project implements a thread-safe, lexicographically sorted doubly linked list that stores pointers to user-supplied strings.<br> POSIX mutexes and condition variables are preventing deadlocks and unexpected bugs allowing concurrent acces.
# Features
- creating list with given capacity<br>
- removing list and freeing memory it occupied<br>
- adding strings to the list in lexicographical order<br>
- removing the first/the last string<br>
- removing string by pointer<br>
- counting and printing list<br>
- removing duplicates and setting new capacity<br>
- joining two lists into one<br>
- removing all strings from list but keeping the structure
# Concurrent part
Every function safely handles multiple threads accessing the list at once and, if needed, sends signals about available space or available resources.
# Files and Invoking
slist.h - header file defining list_t struct<br>
slist.c - implementation of the list with synchronization<br>
main.c - demonstration of library by creating threads and concurrently manipulating the lists<br>
compile the project using: **cc -pthread slist.c main.c**
# Resources 
- [https://www.geeksforgeeks.org] - help with some difficult functions
- [https://stackoverflow.com/questions] - explanations of mutexes and concurrency
- [https://www.cs.put.poznan.pl/csobaniec/index.html] - almost all knowledge learned from my professor
