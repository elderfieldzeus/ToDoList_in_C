## A personal To-Do List program written in C programming Language (Linked List Implementation)
### How to link program.c to todo_list.c:

To link two c programs using gcc, enter this line in the terminal:

```
gcc <name_of_program> <name_of_program>
```

In our case it is:


```
gcc ./program.c ./header/todo_list.c
```

To run the file, run the line/s below:

Windows:
```
.\a.exe
```

Linux:
```
./a.out
```

Enjoy haha

<hr/>

### Self Notes:
 - ``` remove() and rename() ``` functions do not work unless you do ``` fclose(fptr) ``` beforehand (This is for windows. It works on Linux tho idky haha).
 - when passing ```FILE *fptr```, should always pass it by address when wishing to do another ```fopen()```;
