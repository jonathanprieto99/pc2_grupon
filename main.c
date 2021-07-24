#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define COMMAND_LINE_SIZE 50
#define MEM_SIZE 100
struct memory{
    char pid[2];
    int bottom;
    int top;
};

struct memory* contiguous_memory[MEM_SIZE]; // pointer to array
int blocks; // array size

void copy_mem(struct memory* updated_mem[MEM_SIZE]){
    for(int i = 0; i < blocks; i++){
        contiguous_memory[i] = updated_mem[i];
    }
}

struct memory* set_mem_values(char new_pid[2], int new_bottom,int new_top){
    struct memory *new_block = (struct memory*) malloc(sizeof(struct memory));
    new_block->bottom = new_bottom;
    new_block->top = new_top;
    memcpy(new_block->pid, new_pid, sizeof(char) * 2);
    return new_block;
}

void update_mem(struct memory* new_block, struct memory* new_unused_block){
    struct memory* temp; 
    struct memory* updated_mem[MEM_SIZE];
    int j=0;
    for(int i = 0; i < blocks; i++){
        temp = contiguous_memory[i];
        if(temp->bottom<new_block->bottom && new_block->top>temp->top){
            updated_mem[j] = temp;
        } else{
            updated_mem[j] = new_block;
            updated_mem[j+1] = new_unused_block;
            j+=2;
        }
        j++;
    }
    blocks++;
    copy_mem(updated_mem);   
}

int first_fit(char process[2], int size){
    struct memory* current;
    int process_in_mem = 0;
    for(int i = 0; i < blocks; i++){
        current = contiguous_memory[i];
        int temp = current->top - current->bottom;
        if(temp >= size && current->pid[0] == 'U'){
            struct memory* new_block;
            if(temp == size){
                new_block = set_mem_values(process,current->bottom,current->top);
                contiguous_memory[i] = new_block;
                process_in_mem = 1;
                break;
            } else{
                new_block = set_mem_values(process,current->bottom,current->bottom+size);
                struct memory *new_unused_block = set_mem_values("U ",new_block->top+1,current->top);            
                update_mem(new_block,new_unused_block);
                process_in_mem = 1;
                break;
            }
        }
    }
    return process_in_mem;
}

int best_fit(char process[2], int size){
    struct memory* current;
    int process_in_mem = 0;
    int best = 0;
    int min_value = 100000000;
    struct memory* new_block;
    for(int i = 0; i < blocks; i++){
        current = contiguous_memory[i];
        int temp = current->top - current->bottom;
        if(temp >= size && current->pid[0] == 'U'){
            if(temp == size){
                best = i;
                min_value = temp;
                new_block = set_mem_values(process,current->bottom,current->top);
                contiguous_memory[i] = new_block;
                process_in_mem = 1;
                return process_in_mem;
            } else if(temp < min_value){
                best = i;
                min_value = temp;
            }
        }
    }

    if(best < blocks){
        new_block = set_mem_values(process, contiguous_memory[best]->bottom, contiguous_memory[best]->bottom + size);
        struct memory* new_unused_block = set_mem_values("U ", new_block->top + 1, contiguous_memory[best]->top);
        update_mem(new_block, new_unused_block);
        process_in_mem = 1;
    }

    return process_in_mem;
}

int worst_fit(char process[2], int size){
    struct memory* current;
    int process_in_mem = 0;
    int worst = 0;
    int max_value = 0;
    struct memory* new_block;
    for(int i = 0; i < blocks; i++){
        current = contiguous_memory[i];
        int temp = current->top - current->bottom;
        if(temp >= size && current->pid[0] == 'U'){
            if(temp > max_value){
                worst = i;
                max_value = temp;
            }
        }
    }
    
    if(worst < blocks){
        new_block = set_mem_values(process, contiguous_memory[worst]->bottom, contiguous_memory[worst]->bottom + size);
        struct memory* new_unused_block = set_mem_values("U ", new_block->top + 1, contiguous_memory[worst]->top);
        update_mem(new_block, new_unused_block);
        process_in_mem = 1;
    }

    return process_in_mem;
}


void request(char process[2], int size, char typefit[1]){
    printf("Request for process: %s of size: %d with a fit of type: %s\n", process, size, typefit);
    int status;
    switch(typefit[0]){
        case 'F':
            status = first_fit(process,size);
            break;
        case 'B':
            status = best_fit(process,size);
            break;
        case 'W':
            status = worst_fit(process,size);
            break;
        default:
            printf("ERROR: Fit type not found.\n");
            return;
    }
    if(status != 1) printf("ERROR: Not enough space for allocate the process\n");
}

struct memory* set_combination(struct memory* mem1, struct memory* mem2){
    struct memory* new_combination = set_mem_values("U ",mem1->bottom,mem2->top);
    free(mem1);
    free(mem2);
    blocks--;
    return new_combination;
}

void combine_blocks(){
    struct memory* temp_bottom;
    struct memory* temp_top;
    struct memory* updated_mem[MEM_SIZE];
    int already_updated = 0, j=0;
    for(int i = 0; i<blocks; i++){
        temp_bottom = contiguous_memory[i];
        if(temp_bottom->pid[0]=='U' && (j+1)<blocks){
            //j++;
            temp_top = contiguous_memory[j+1];
            while(temp_top->pid[0]=='U' && (j+1)<blocks){
                already_updated = 1;
                j++;
                temp_top = contiguous_memory[j];
                blocks--;
            }
            if(already_updated == 1){
                updated_mem[i] = set_combination(temp_bottom,temp_top);
                i = j;
            } else j--;
        }
        if (already_updated == 0){
            updated_mem[i] = temp_bottom;
            j=i+1;
        }
    }
    copy_mem(updated_mem);    
}

void release_mem(struct memory* new_unused_block){
    struct memory* temp; 
    struct memory* updated_mem[MEM_SIZE];
    for(int i = 0; i < blocks; i++){
        temp = contiguous_memory[i];
        if(temp->bottom==new_unused_block->bottom){
            updated_mem[i] = new_unused_block;
        } else {
            updated_mem[i] = temp;
        }
    }
    copy_mem(updated_mem);   
    combine_blocks();
}


void release(char process[2]){
    printf("Realeasing process: %s\n",process);
    struct memory* temp;
    for(int i = 0; i < blocks; i++){
        temp = contiguous_memory[i];
        if(temp->pid[0] == 'P'){
            if(strcmp(temp->pid,process)==0){
                struct memory *new_unused_mem = set_mem_values("U ",temp->bottom,temp->top);
                //combine_blocks(new_unused_mem);
                release_mem(new_unused_mem);
                break;
            }
        }
    }
}

void compact(){
    /* printf("Enters Compact\n");
    struct memory* temp;
    for(int i = 0; i < blocks; i++){
        temp = contiguous_memory[i];
        if(temp->pid[0] == 'F' && (temp + 1)->pid[0] == 'U'){
            struct memory new_part;
            new_part.bottom = temp->bottom;
            new_part.top = (temp + 1)->top;
            new_part.pid[0] = 'U';
            int new_partitions = blocks - 1;
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
    } */
}

void status_report(){
    printf("Enters Status Report\n");
    struct memory* temp; 
    for(int i = 0; i < blocks; i++){
        temp = contiguous_memory[i];
        if(temp->pid[0] == 'P'){
            printf("Addresses   [%d:%d] Process %s\n", temp->bottom, temp->top, temp->pid);
        } else{
            printf("Addresses   [%d:%d] Unused\n", temp->bottom, temp->top);
        }
    }
}

void allocate_memory(int size){
    printf("The size of the memory will be: %d\n", size);
    struct memory *block = malloc(sizeof(struct memory));
    block->bottom = 0;
    block->top = size;
    block->pid[0] = 'U';
    contiguous_memory[0] = block;
    blocks = 1;
}

void options(int mem_size){
    char command_line[COMMAND_LINE_SIZE];

    while(1){
        printf("allocator> ");
        scanf(" %[^\t\n]s",command_line);

        char* arg;
        char* temp = command_line;
        char* args[COMMAND_LINE_SIZE];
        int args_len=0;
        while((arg = strtok_r(temp," ",&temp))){
            args[args_len] = arg;
            args_len++;
        }

        if(strcmp(args[0], "STAT") == 0){
            status_report();
        }
        else if(strcmp(args[0],"RQ") == 0){
            int block_size = atoi(args[2]);
            if(block_size>mem_size){
                printf("ERROR: Exceeded size.\n");
            } else request(args[1], block_size, args[3]);
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
    options(size);

    return 0;
}
