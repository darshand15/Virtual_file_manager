#include<headers.h>

void init_space(void *temp, int n)
{
    temp = (void*)malloc(sizeof(n)); //page allocation

    if(temp==NULL)
    {
        printf("No more space available\n");
        return;
    }
    mem_space *m = (mem_space*)temp;
    m->free_size = n - sizeof(mem_space);
    m->files = -1;
    m->free_head = -1;

}

void init_manager(void *temp)
{
    init_space(temp,1000);
}

void create_file(const char* file_name, void *temp)
{
    mem_space *m = (mem_space*)temp;
    if(m->files==-1)
    {
        m->files = sizeof(mem_space);
        file_header *fh = (file_header*)(temp + m->files);
        
    }
}

