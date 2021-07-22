#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void request(char process[2], int size, char typefit[1]){
    printf("Se hace un request para el proceso: %s de tamaño: %d con un fit de tipo: %s\n", process, size, typefit);
}

void release(char process[2]){
    printf("Se hace el release del proceso: %s\n",process);
}

void compact(){
    printf("Entro a Compact\n");
}

void status_report(){
    printf("Entro a Reporte de Status\n");
}

void allocate_memory(int size){
    printf("The size of the memory will be: %d\n", size);
}

int main() {

    char str[8];
    gets( str );
    int size = atoi(str);

    allocate_memory(size);

    char process[3];
    char command[5];
    int bytes;
    char type_fit[2];

    while(1){

        printf("allocator>");
        scanf("%s %s %d %s" , command, process, &bytes, type_fit);
        //printf(command);

        if (strcmp(command, "STAT") == 0){

            status_report();

        }

        else if(strcmp(command,"RQ") == 0){

            request(process, bytes, type_fit);

        }

        else if(strcmp(command,"RL") == 0){

            release(process);

        }

        else if(strcmp(command, "C") == 0){

            compact();

        }

        else if(strcmp(command, "X") == 0){

            break;

        }

        continue;

    }



    return 0;
}
