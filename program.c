#include "./header/todo_list.h"

int main() {
    FILE *file = openFile();
    List head = getList(file);
    Choice option = landingScreen();
    
    while(option != EXIT) {
        doChoice(option, &head, &file);
        option = landingScreen();
    } 

    saveToFile(&file, head);
    endProgram(file, head);
    return 0;
}
