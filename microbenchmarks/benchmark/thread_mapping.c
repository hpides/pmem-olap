#include <stdio.h>
#include "thread_mapping.h"
#include "common.h"

void create_thread_mapping(struct access_info *ac_info){
    if(!ac_info->pinned){
        return;
    }
    FILE *mapping_file;
    mapping_file = fopen(ac_info->mapping_path, "r");
    if(!mapping_file){
        perror("Thread mapping file not found.");
        exit(EXIT_FAILURE);
    }
    
    char* line = NULL;
    size_t len;
    ssize_t read;
    int thread_index = 0;

    while (thread_index < ac_info->thread_count && (read = getline(&line, &len, mapping_file)) != -1 ) {
        ac_info->thread_mapping[thread_index] = atoi(line);
        thread_index++;
    }

    if(read == -1){
        printf("Error: There were unsufficient mappings in the mapping file.\n");
        exit(EXIT_FAILURE);
    }

    fclose(mapping_file);
    free(line);

    return;
}
    