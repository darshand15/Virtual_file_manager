#include "headers.h"

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
    m->size_free = n - sizeof(mem_space);
    m->free_head = -1;
    m->file_offset = -1;
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


void create_file(const char* filename, const char* filetype, void* temp)
{
    mem_space* m = (mem_space*)temp;
    if(m->file_offset == -1)
    {
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
    }
    else
    {
        
    }
    
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
