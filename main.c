#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define COMMAND_LINE_SIZE 50
struct memory{
    char pid[2];
    int bottom;
    int top;
};

struct memory *contiguous_memory; // pointer to array
int partitions; // array size


void first_fit(char process[2], int size){
    struct memory* current = contiguous_memory;
    for(int i = 0; i < partitions; i++){
        int temp = current->bottom + current->top;
        if(temp >= size && current->pid[0] == 'U'){
            struct memory *new_block = (struct memory*) malloc(sizeof(struct memory));
            if(temp == size){
                new_block->bottom = current->bottom;
                new_block->top = current->top;
                memcpy(new_block->pid, process, sizeof(char) * 2);
                current = new_block;
                break;
            } else{
                if(partitions==0)
                    new_block->bottom = current->bottom+1;
                else new_block->bottom = current->bottom;
                new_block->top = current->bottom + size;
                memcpy(new_block->pid, process, sizeof(char) * 2);
                partitions++;
                struct memory *new_partition;
                new_partition->bottom = new_block->top + 1;
                new_partition->top = current->top;
                new_partition->pid[0] = 'U';
                struct memory new_array[partitions];
                struct memory* iterator = contiguous_memory;
                bool partition_added = false;
                for(int j = 0; j < partitions; j++){
                    if(j == i + 1){
                        new_array[j] = *new_partition;
                        partition_added = true;
                    } else if(j == i){
                        new_array[j] = *new_block;
                    } else if(partition_added) new_array[j] = *(iterator - 1);
                    else{
                        printf("OK");
                        new_array[j] = *iterator;
                    }
                }
                *contiguous_memory = *new_array;
                break;
            }
        }
        current++;
    }
}

void best_fit(char process[2], int size){
    int best = 0;
    int min_value = 100000000;
    struct memory* current2 = contiguous_memory;
    struct memory* current = contiguous_memory;
    for(int i = 0; i < partitions; i++){
        int temp = current2->bottom + current2->top;
        if(temp >= size && current2->pid[0] == 'U'){
            if(temp == size){
                best = i;
                min_value = temp;
                break;
            } else if(temp < min_value){
                best = i;
                min_value = temp;
            }
        }
        current2++;
    }
    for(int i = 0; i <= best; i++){
        ++current;
    }
    int temp = current->bottom + current->top;
    struct memory *new_block = (struct memory*) malloc(sizeof(struct memory));
    new_block->bottom = current->bottom+1;
    new_block->top = current->bottom + size;
    memcpy(new_block->pid, process, sizeof(char) * 2);
    partitions++;
    struct memory new_partition;
    new_partition.bottom = new_block->top + 1;
    new_partition.top = (temp - size) + new_block->top;
    new_partition.pid[0] = 'U';
    struct memory new_array[partitions];
    struct memory* iterator = contiguous_memory;
    bool partition_added = false;
    for(int i = 0; i < partitions; i++){
        if(i == best){
            new_array[i] = *new_block;
            partition_added = true;
        }
        else if(i == best + 1) new_array[i] = new_partition;
        else if(partition_added) new_array[i] = *(iterator - 1);
        else new_array[i] = *iterator;
    }
    contiguous_memory = new_array;
}

void worst_fit(char process[2], int size){
    int worst = 0;
    int max_value = 0;
    struct memory* current2 = contiguous_memory;
    struct memory* current = contiguous_memory;
    for(int i = 0; i < partitions; i++){
        int temp = current2->bottom + current2->top;
        if(temp >= size && current2->pid[0] == 'U'){
            if(temp > max_value){
                worst = i;
                max_value = temp;
            }
        }
        current2++;
    }
    for(int i = 0; i < worst; i++){
        current++;
    }
    int temp = current->bottom + current->top;
    struct memory *new_block = (struct memory*) malloc(sizeof(struct memory));
    new_block->bottom = current->bottom+1;
    new_block->top = current->bottom + size;
    memcpy(new_block->pid, process, sizeof(char) * 2);
    partitions++;
    struct memory new_partition;
    new_partition.bottom = new_block->top + 1;
    new_partition.top = (temp - size) + new_block->top;
    new_partition.pid[0] = 'U';
    struct memory new_array[partitions];
    struct memory* iterator = contiguous_memory;
    bool partition_added = false;
    for(int i = 0; i < partitions; i++){
        if(i == worst){
            new_array[i] = *new_block;
            partition_added = true;
        }
        else if(i == worst + 1) new_array[i] = new_partition;
        else if(partition_added) new_array[i] = *(iterator - 1);
        else new_array[i] = *iterator;
        iterator++;
    }
    contiguous_memory = new_array;
}


void request(char process[2], int size, char typefit[1]){
    printf("Request for process: %s of size: %d with a fit of type: %s\n", process, size, typefit);
    switch(typefit[0]){
        case 'F':
            first_fit(process,size);
            break;
        case 'B':
            best_fit(process,size);
            break;
        case 'W':
            worst_fit(process,size);
            break;
        default:
            return;
    }
}

void release(char process[2]){
    printf("Realeasing process: %s\n",process);
    struct memory* temp = contiguous_memory;
    for(int i = 0; i < partitions; i++){
        if(temp->pid[0] == 'P'){
            if(strcmp(temp->pid,process)){
                struct memory new_free;
                new_free.bottom = temp->bottom;
                new_free.top = temp->top;
                new_free.pid[0] = 'U';
                free(temp);
                temp = &new_free;
                break;
            }
        }
        temp++;
    }
}

void compact(){
    printf("Enters Compact\n");
    struct memory* temp = contiguous_memory;
    for(int i = 0; i < partitions; i++){
        if(temp->pid[0] == 'F' && (temp + 1)->pid[0] == 'U'){
            struct memory new_part;
            new_part.bottom = temp->bottom;
            new_part.top = (temp + 1)->top;
            new_part.pid[0] = 'U';
            int new_partitions = partitions - 1;
            struct memory* new_array[new_partitions];
            bool partition_added = false;
            struct memory* current = contiguous_memory;
            for(int j = 0; j < new_partitions; j++){
                if(j == i){
                    new_array[i] = &new_part;
                    partition_added = true;
                }else if(partition_added) new_array[i] = (current + 1);
                else new_array[i] = current;
                current++;
            }
        }
    }
}

void status_report(){
    printf("Enters Status Report\n");
    struct memory temp; 
    for(int i = 0; i < partitions; i++){
        temp = contiguous_memory[i];
        if (temp.pid[0] == 'P'){
            printf("Addresses   [%d:%d] Process %s\n", temp.bottom, temp.top, temp.pid);
        }

        else{
            printf("Addresses   [%d:%d] Unused\n", temp.bottom, temp.top);
        }
        //temp++;
    }
}

void allocate_memory(int size){
    printf("The size of the memory will be: %d\n", size);
    struct memory *block = malloc(sizeof(struct memory));
    block->bottom = 0;
    block->top = size;
    block->pid[0] = 'U';
    contiguous_memory = block;
    partitions = 1;
}

void options(){
    char command_line[COMMAND_LINE_SIZE];

    while(1){

        printf("allocator> ");
        scanf(" %[^\t\n]s",command_line);
        printf("%s",command_line);

        char* arg;
        char* temp = command_line;
        char* args[COMMAND_LINE_SIZE];
        int args_len=0;
        while((arg = strtok_r(temp," ",&temp))){
            args[args_len] = arg;
            args_len++;
        }

        if (strcmp(args[0], "STAT") == 0){
            status_report();
        }
        else if(strcmp(args[0],"RQ") == 0){
            request(args[1], atoi(args[2]), args[3]);
        }
        else if(strcmp(args[0],"RL") == 0){
            release(args[1]);
        }
        else if(strcmp(args[0], "C") == 0){
            compact();
        }
        else if(strcmp(args[0], "X") == 0){
            break;
        } 
        else{
            printf("ERROR: That command does not exist.\n");
        }
        continue;
    }
}

int main() {

    char str[8];
    printf("./allocator ");
    scanf("%s", str);
    int size = atoi(str);

    allocate_memory(size);

    options();


    return 0;
}
