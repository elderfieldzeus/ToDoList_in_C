#include "./todo_list.h"

const char *filename = "./files/tasks.dat";

FILE *openFile() {
    FILE *fptr;

    if((fptr =  fopen(filename, "rb+")) == NULL) {
        fptr = fopen(filename, "wb+");
        // printf("New File has been made!");
    }
    else {
        fptr = fopen(filename, "ab+");
    }
    
    return fptr;
}

List getList(FILE *fptr) {
    List head;
    List *curr = &head;
    Task temp;

    rewind(fptr);

    while(fread(&temp, sizeof(Task), 1, fptr) != 0) {
        (*curr) = (List)malloc(sizeof(struct node));
        if((*curr) != NULL) {
            (*curr)->task = temp;
            curr = &(*curr)->next;
        }
    }

    (*curr) = NULL;

    return head;
}

Choice landingScreen() {
    int choice;
    Choice option;

    printf("Welcome to your to-do list (DONT FORGET TO SAVE BEFORE EXITING)\n");
    printf("[1] Read Task/s\n");
    printf("[2] Add Task\n");
    printf("[3] Update Task Status\n");
    printf("[4] Delete Task\n");
    printf("[?] Save & Exit\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1: option = READ; break;
        case 2: option = ADD; break;
        case 3: option = UPDATE; break;
        case 4: option = DELETE; break;
        default: option = EXIT; 
    } 

    return option;
}

void doChoice(Choice option, List *head) {
    switch(option) {
        case READ: displayList(*head); break;
        case ADD: addToList(head); break;
        case UPDATE: updateStatus(*head); break;
        case DELETE: deleteFromList(head); break;
    }
} 

void displayList(List head) {
    printf("\033[H\033[J");
    if(head != NULL) {
        printf("%-30s%-19s%-15s%-12s\n", "Task", "Deadline", "Status", "Priority");

        for(int i = 0; i < 73; i++) {
            printf("-");
        }

        printf("\n");

        for(List curr = head; curr != NULL; curr = curr->next) {
            char status[12], urgency[12] = "";

            switch(curr->task.status) {
                case UNFINISHED: strcpy(status, "Not Done"); break;
                case PENDING: strcpy(status, "In Progress"); break;
                case DONE: strcpy(status, "Done"); 
            }

            switch(curr->task.priority) {
                case LOW: strcpy(urgency, "LOW"); break;
                case MEDIUM: strcpy(urgency, "MEDIUM");break;
                case HIGH: strcpy(urgency, "HIGH");
            } 

            char date[20] = "";

            if(curr->task.deadline.day != -1) {
                sprintf(date, "%s %02d, %4d", curr->task.deadline.month, curr->task.deadline.day, curr->task.deadline.year);
            }
    
            printf("%-30s%-19s%-15s%-12s\n", curr->task.description, date, status, urgency);
        }
    }
    else {
        printf("No tasks are available. Add a task!\n");
    }
    printf("\n\n");
}

int monthValid(char month[]) {
    char calendar[][10] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    int i;

    month[0] = toupper(month[0]);

    for(i = 1; month[i] != '\0'; i++) {
        month[i] = tolower(month[i]);
    }

    for(i = 0; i < 12 && strcmp(calendar[i], month) != 0; i++) {}

    if(i == 12) {
        printf("Month invalid... try again\n");
    }

    return (i == 12) ? 0 : i + 1;
}

int dateValid(int month, int day) {
    int monthDays[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return (day < monthDays[month - 1] && day > 0) ? 1 : 0;
}

void capitalize(char *sentence) {
    sentence[0] = toupper(sentence[0]);
    for(int i = 1; sentence[i] != '\0'; i++) {
        if(isspace(sentence[i - 1])) {
            sentence[i] = toupper(sentence[i]);
        }
        else {
            sentence[i] = tolower(sentence[i]);
        }
    }
}

Task createTask() {
    Task temp = {"xxxx", {"x", -1, -1, 10000}, UNFINISHED, NONE};
    char yorn;
    char desc[100];
    int length;

    printf("\033[H\033[J");
    do{
        printf("Enter new task: ");
        scanf(" %[^\n]", desc);
        if((length = strlen(desc) + 1) >= MAX) {
            printf("Task exceeds maximum of %d letters.\n", MAX - 1);
        }
    }while(length >= MAX);

    strcpy(temp.description, desc);
    capitalize(temp.description);

    printf("\033[H\033[J");
    printf("Is there a deadline? [y/n]: ");
    scanf(" %c", &yorn);

    if(tolower(yorn) == 'y') {
        char month[10];
        int mNum;
        printf("\033[H\033[J");
        printf("Deadline:\n\n");
        do{
            printf("Enter month: ");
            scanf(" %s", month);
        }while((mNum = monthValid(month)) == 0);

        strcpy(temp.deadline.month, month);
        temp.deadline.month_in_num = mNum;

        int date;
        do {
            printf("Enter day: ");
            scanf("%d", &date);
        }while (dateValid(mNum, date) == 0);

        temp.deadline.day = date;

        int year;
        do {
            printf("Enter year: ");
            scanf("%d", &year);
        } while(year <= 0 && year >= 10000);

        temp.deadline.year = year;
    }

    int urgency;

    printf("\033[H\033[J");
    printf("How much of a priority is this?\n");
    printf("[1] High\n");
    printf("[2] Medium\n");
    printf("[3] Low\n");
    printf("[?] None\n");
    printf("Urgency: ");
    scanf("%d", &urgency);

    switch(urgency) {
        case 1: temp.priority = HIGH; break;
        case 2: temp.priority = MEDIUM; break;
        case 3: temp.priority = LOW; break;
        default: temp.priority = NONE;
    }

    printf("\033[H\033[J");

    return temp;
}

void addToList(List *head) {
    Task new_task = createTask();
    
    List *curr;
    List temp = (List) malloc(sizeof(struct node));

    if(temp != NULL) {
        for(curr = head; (*curr) != NULL && new_task.priority < (*curr)->task.priority; curr = &(*curr)->next) {}
        for(; (*curr) != NULL && new_task.deadline.year > (*curr)->task.deadline.year && new_task.priority == (*curr)->task.priority; curr = &(*curr)->next) {} //find proper year
        for(; (*curr) != NULL && new_task.deadline.month_in_num > (*curr)->task.deadline.month_in_num && new_task.deadline.year > (*curr)->task.deadline.year && new_task.priority == (*curr)->task.priority; curr = &(*curr)->next) {} //find proper month
        for(; (*curr) != NULL && new_task.deadline.day > (*curr)->task.deadline.day && new_task.deadline.year > (*curr)->task.deadline.year && new_task.priority == (*curr)->task.priority && new_task.deadline.month_in_num == (*curr)->task.deadline.month_in_num; curr = &(*curr)->next) {} //find proper date
        
        temp->task = new_task;
        temp->next = *curr;
        *curr = temp;
    } 
}

List findToBeEdited(List head, Sentence find) {
    List curr;
    for(curr = head; curr != NULL && strcmp(curr->task.description, find) != 0; curr = curr->next) {}

    return curr;
}

void updateStatus(List head) {
    List edit;
    Sentence find;

    printf("\033[H\033[J");
    printf("Enter task to be updated: ");
    scanf(" %[^\n]", find);
    capitalize(find);

    edit = findToBeEdited(head, find);
    if(edit == NULL) {
        printf("Couldn't find task with that description.\n\n");
    }
    else {
        int choice;
        printf("\033[H\033[J");
        printf("What would you like to change the status to?\n");
        printf("[1] Not Done\n");
        printf("[2] In Progress\n");
        printf("[3] Done\n");
        printf("Choice: ");
        scanf("%d", &choice);

        printf("\033[H\033[J");

        switch(choice) {
            case 1: edit->task.status = UNFINISHED; break;
            case 2: edit->task.status = PENDING; break;
            case 3: edit->task.status = DONE; break;
            case 4: printf("Not an option.\n\n");
        }
    }
}

void deleteFromList(List *head) {
    Sentence delete;
    printf("\033[H\033[J");
    printf("Enter task to be deleted: ");
    scanf(" %[^\n]", delete);

    capitalize(delete);

    List *curr = head, temp;
    int deleted = 0;

    while(*curr != NULL) {
        if(strcmp((*curr)->task.description, delete) == 0) {
        temp = *curr;
        *curr = temp->next;
        free(temp);
        deleted = 1;
        }
        else {
            curr = &(*curr)->next;
        }
    }

    if(deleted == 1) {
        printf("Deleted.\n\n");
    }
    else {
        printf("Couldn't find task with that description.\n\n");
    }

}




void saveToFile(FILE *fptr, List head) {
    FILE *new_file = fopen("./files/temp", "wb+");

    while(head != NULL) {
        fwrite(&(head)->task, sizeof(Task), 1, new_file);
        head = head->next;
    }

    remove(filename);
    rename("./files/temp", filename);

    fptr = new_file;
}

void endProgram(FILE *fptr, List head) {
    List prev;
    while(head != NULL) {
        prev = head;
        head = head->next;
        free(prev);
    }

    fclose(fptr);

    printf("\033[H\033[J");
    printf("Thanks for using my version of To-do list.\n\n");
    printf("\u00A9 Zeus D. Elderfield\n\n");
}