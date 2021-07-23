#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct memory{
    char pid[2];
    int bottom;
    int top;
};

struct memory *contiguous_memory; // pointer to array
int partitions; // array size


void first_fit(char process[2], int size){
    for(int i = 0; i < partitions; i++){
        int temp = contiguous_memory->bottom + contiguous_memory->top;
        if(temp >= size && strcmp(contiguous_memory->pid[0], 'F')){
            struct memory *new_block = (struct memory*) malloc(sizeof(struct memory));
            if(temp == size){
                new_block->bottom = contiguous_memory->bottom;
                new_block->top = contiguous_memory->top;
                memcpy(new_block->pid, process, sizeof(char) * 2);
            } else{
                new_block->bottom = contiguous_memory->bottom+1;
                new_block->top = contiguous_memory->bottom + size;
                memcpy(new_block->pid, process, sizeof(char) * 2);
                partitions++;
                struct memory new_partition;
                new_partition.bottom = new_block->top + 1;
                new_partition.top = (temp - size) + new_block->top;
                new_partition.pid[0] = 'F';
                struct memory new_array[partitions];
                for(int j = 0; j < partitions; j++){
                    if(j == i + 1){
                        new_array[j] = new_partition;
                    } else new_array[j] = contiguous_memory[j];
                }
                contiguous_memory = new_array;
            }
        }
    }
}

void best_fit(){

}

void worst_fit(){

}




void request(char process[2], int size, char typefit[1]){
    printf("Request for process: %s of size: %d with a fit of type: %s\n", process, size, typefit);
    switch(typefit[0]){
        case 'F':
            first_fit(process,size);
            break;
        case 'B':
            best_fit();
            break;
        case 'W':
            worst_fit();
            break;
        default:
            return;
    }
}

void release(char process[2]){
    printf("Realeasing process: %s\n",process);
}

void compact(){
    printf("Enters Compact\n");
}

void status_report(){
    printf("Enters Status Report\n");
    struct memory *temp = contiguous_memory;
    for(int i = 0; i < partitions; i++){
        if(strcmp(temp->pid[0], 'P') == 0)
            printf("Addresses   [%d:%d] Process %s\n", temp->bottom, temp->top, temp->pid);
        else printf("Addresses   [%d:%d] Unused\n", temp->bottom, temp->top);
        temp++;
    }
}

void allocate_memory(int size){
    printf("The size of the memory will be: %d\n", size);
    struct memory block;
    block.bottom = 0;
    block.top = size;
    block.pid[0] = 'F';
    contiguous_memory = &block;
    partitions = 1;
    // int random_size = rand() % 25 + 5;
    // int random_size = 10;
    // int bottoms[random_size];
    // bottoms[0] = 0;
    // struct memory array[random_size];
    // int mem_remaining = size;
    // int mem_remaining_temp = size;
    // // for(int i = 0; i < random_size; i++){
    // //     struct memory temp;
    // //     int partition = rand() % random_size / 3 + 1;
    // //     int r = rand() % mem_remaining / partition;
    // //     if(i == 0) temp.bottom = 0;
    // //     else temp.bottom = array[i - 1].top + 1;
    // //     if (i == 9)temp.top = size;
    // //     else temp.top = temp.bottom + r;
    // //     array[i] = temp;
    // //     mem_remaining -= r;
    // //     printf("[%d, %d]\n", array[i].bottom, array[i].top);
    // // }
    // int remaining = size;
    // int suma = 0;
    // for(int i = 0; i < random_size; i++){
    //     // int partition = rand() % random_size / 3 + 1;
    //     int partition = rand() % remaining / 2 + 1;
    //     if(i == random_size - 1) partition = remaining;
    //     //Porcentajes, luego multiplicarlo por el size esas 10 veces y obtengo las particiones
    //     // int r = rand() % mem_remaining_temp / partition;
    //     printf("Partition %d\n", partition);
    //     suma += partition;
    //     remaining -= partition;
    // }
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
