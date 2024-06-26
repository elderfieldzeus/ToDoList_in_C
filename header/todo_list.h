#ifndef TODO_HEADER
#define TODO_HEADER

#define MAX 31 //including '\0'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// User Datatypes

typedef char Sentence[MAX];

typedef enum {
    READ, ADD, SORT, DELETE, EDIT, CLEAR, EXIT
} Choice;

typedef enum {
    UNFINISHED, PENDING, DONE
} Status;

typedef enum {
    NONE, LOW, MEDIUM, HIGH
} Importance;

typedef struct {
    char month[10];
    int month_in_num;
    int day;
    int year;
} Date;

typedef struct {
    Sentence description;
    Date deadline; // 00/00/0000
    Status status;
    Importance priority;
} Task;

typedef struct node {
    Task task;
    struct node *next;
} *List;

// Functions

FILE *openFile();
List getList(FILE *fptr);
Choice landingScreen();
void doChoice(Choice option, List *head, FILE **file);
void displayList(List head);
int monthValid(char month[]);
int dateValid(int month, int day);
Task createTask();
void addToList(List *head);
List findToBeEdited(List head, Sentence find);
void typeOfSorting(List head);
int compareDate(List curr, List key);
void sortByDate(List head);
void sortByPriority(List head);
//void updateStatus(List head);
void editTask(List head);
void editDescription(List edit);
void editDeadline(List edit);
void editPriority(List edit);
void editStatus(List head);
void deleteFromList(List *head);
void clearTask(FILE **fptr, List *head);
void freeAll(List head);
void saveToFile(FILE **fptr, List head);
void endProgram(FILE *fptr, List head);

#endif
