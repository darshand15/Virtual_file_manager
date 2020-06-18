#include "headers_M.h"

int exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void init_space(void* temp, int n)
{
    temp = malloc(sizeof(n));
    if(temp == NULL)
    {
        printf("No More Space Available\n");
        return;
    }
    mem_space* m = (mem_space*)temp;
    m->size = n - sizeof(mem_space);
    m->alloc = 0;
    m->free_head = -1;
    m->file_offset = -1;
    m->next = -1;
    m->current_offset = 0;
}
void init_manager(void* temp)
{
    char* filename = "file_manager.dat";
    if(!exists(filename))
    {
        temp = init_space(temp, 1000);
        FILE* fp = fopen(filename,"wb");
        fclose(fp);
        fp = fopen(filename, "rb+");
        fseek(fp, 0, SEEK_SET);
        fwrite(&temp, 1000, 1, fp);
    }
    
}

mem_space* worst_block(void* temp, int r_size) 
{
  
    mem_space* head_mem_space = (mem_space*)temp;   // Initialise a pointer to the head mem_space

    /* Get largest unallocated block in list*/

    mem_space* index = head_mem_space, *max = NULL;   // max pointer would point to the block with largest unallocated space
    do {
            if(!(index->alloc) && (!max || index->size > max->size) ) // if unallocated and size is greater than max size
            {
                max = index;
            }
            index = (temp + index->next);
    } while(index != NULL);

    if(!max||max->size >= r_size)     // return max block only if the size is greater than the required size
    {
        return max;
    } 
    else
    {
        return NULL;            // return NULL if an appropriate block is not found
    }
}

void create_file(const char* filename, const char* filetype, void* temp)
{
    // mem_space* m = (mem_space*)temp;
    mem_space* m = worst_block(temp, sizeof(file_header));
    // if(m->file_offset == -1)
    // {
    //     m->file_offset = sizeof(mem_space);
    //     file_header* file_h = (file_header*)(temp + m->file_offset);
    //     file_h->file_id = 0;
    //     file_h->file_name = malloc(sizeof(char)*(strlen(filename)+1));
    //     strcpy(file_h->file_name, filename);
    //     file_h->file_type = malloc(sizeof(char)*(strlen(filetype)+1));
    //     strcpy(file_h->file_type, filetype);
    //     file_h->start_offset = -1;
    //     file_h->end_offset = -1;
    //     file_h->prev = -1;
    //     file_h->next = -1;
    //     m->alloc = 1;
    //     if(m->size > (sizeof(file_header) + sizeof(mem_space)))
    //     {
    //         int offset = sizeof(mem_space) + sizeof(file_header);
    //         mem_space* free_m = (mem_space*)(m + offset);
    //         free_m->next = m->next;
    //         m->next = offset;
    //         m->free_head = offset;
    //         free_m->size = m->size - sizeof(file_header) - sizeof(mem_space);
    //         free_m->alloc = 0;
    //         m->size = sizeof(file_header);
    //     }

    // }
    // else
    // {
    //     if(m->size_free > sizeof(file_header))
    //     {
            
    //     }
        
    // }
    if(!m)
    {

    }
    if(m->size > (sizeof(file_header) + sizeof(mem_space)))
    {
        int offset = m->current_offset + sizeof(mem_space) + sizeof(file_header);
        mem_space* free_m = (mem_space*)(m + offset);
        free_m->next = m->next;
        m->next = offset;
        m->free_head = offset;
        free_m->size = m->size - sizeof(file_header) - sizeof(mem_space);
        free_m->alloc = 0;
        m->size = sizeof(file_header);
        free_m->current_offset = m->next;
        free_m->free_head = -1;
        free_m->file_offset = -1;
    }
    m->file_offset = sizeof(mem_space);
    file_header* file_h = (file_header*)(temp + m->file_offset);
    file_h->file_id = 0;
    file_h->file_name = malloc(sizeof(char)*(strlen(filename)+1));
    strcpy(file_h->file_name, filename);
    file_h->file_type = malloc(sizeof(char)*(strlen(filetype)+1));
    strcpy(file_h->file_type, filetype);
    file_h->start_offset = -1;
    file_h->end_offset = -1;
    file_h->prev = -1;
    file_h->next = -1;
    m->alloc = 1;  
}
void insert_into_file(const char* filename, const char* filetype, void* temp)
{
    mem_space* m = (mem_space*)temp;
    if(m->file_offset == -1)
    {
        printf("\nNo file has been created.\n");
        return;
    }
    file_header* temp_file_pointer = (file_header*)(temp + m->file_offset);
    while(temp_file_pointer->next != -1)
    {
        if(strcmp(filename, temp_file_pointer->file_name) && strcmp(filetype, temp_file_pointer->file_type))
        {
            break;
        }
        temp_file_pointer = (temp + temp_file_pointer->end_offset); 
    }

}
