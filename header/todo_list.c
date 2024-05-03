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
    printf("[3] Update Task\n");
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
        case UPDATE: break;
        case DELETE: break;
    }
} 

void displayList(List head) {
    printf("Your tasks:\n\n");
    if(head != NULL) {
        printf("%30s%19s%12s%12s", "Task", "Deadline", "Status", "Urgency\n");
        for(List curr = head; curr != NULL; curr = curr->next) {
            char status[12], urgency[12];

            switch(curr->task.status) {
                case UNFINISHED: strcpy(status, "Not done"); break;
                case PENDING: strcpy(status, "In progress"); break;
                case DONE: strcpy(status, "Done"); 
            }

            switch(curr->task.urgency) {
                case NONE: strcpy(urgency, ""); break;
                case LOW: strcpy(urgency, "Low"); break;
                case MEDIUM: strcpy(urgency, "Medium");break;
                case HIGH: strcpy(urgency, "High"); 
            } 

            if(curr->task.deadline.day == -1) {
                printf("%30s%19s%12s%12s\n", curr->task.description, "", status, urgency);
            }
            else {
                printf("%30s%10s %02d, %4d%12s%12s\n", curr->task.description, curr->task.deadline.month, curr->task.deadline.day, curr->task.deadline.year, status, urgency);
            }
        }
    }
    else {
        printf("No tasks are available. Add a task!\n");
    }    
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

Task createTask() {
    Task temp = {"xxxx", {"x", -1, -1}, UNFINISHED, NONE};
    char yorn;
    printf("Enter new task: ");
    scanf(" %[^\n]", temp.description);
    printf("Is there a deadline? [y/n]: ");
    scanf(" %c", &yorn);

    if(tolower(yorn) == 'y') {
        char month[10];
        int mNum;
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
        } while(year <= 0);

        temp.deadline.year = year;
    }

    int urgency;

    printf("How urgent is this?\n");
    printf("[1] Low\n");
    printf("[2] Medium\n");
    printf("[3] High\n");
    printf("[?] None\n");
    printf("Urgency: ");
    scanf("%d", &urgency);

    switch(urgency) {
        case 1: temp.urgency = LOW; break;
        case 2: temp.urgency = MEDIUM; break;
        case 3: temp.urgency = HIGH; break;
        default: temp.urgency = NONE;
    }

    return temp;
}

void addToList(List *head) {
    Task new_task = createTask();
    
    List *curr;
    List temp = (List) malloc(sizeof(struct node));

    if(temp != NULL) {
        for(curr = head; (*curr) != NULL && new_task.urgency > (*curr)->task.urgency; curr = &(*curr)->next) {} //find urgency
        for(; (*curr) != NULL && strcmp(new_task.description, (*curr)->task.description) > 0 && new_task.urgency == (*curr)->task.urgency; curr = &(*curr)->next) {} //find sorted arrangement amongst
        temp->task = new_task;
        temp->next = *curr;
        *curr = temp;
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
}