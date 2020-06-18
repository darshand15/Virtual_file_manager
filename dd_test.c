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

        int f_n_len = strlen(file_name);
        int flag = 1;
        int len_n,len_ext;
        len_n = 0;
        while(flag && len_n<f_n_len)
        {
            if(file_name[len_n]=='.')
            {
                flag = 0;
            }
            else
            {
                len_n++;
            }
            
        }
        char *name = (char*)malloc((len_n+1)*sizeof(char));
        int co;
        for(co = 0;co<len_n;co++)
        {    
            name[co] = file_name[co];       
        }
        name[len_n] = '\0';
        len_ext = f_n_len - len_n;
        char *ext = (char*)malloc(len_ext*sizeof(char));
        for(co = co+1;co<f_n_len;co++)
        {
            ext[co - len_n - 1] = file_name[co];
        }
        ext[len_ext - 1] = '\0';

        fh->file_id = name;
        fh->start_offset = -1;
        fh->file_type = ext;
        fh->end_offset = -1;
        fh->next = -1;
        fh->prev = -1;
        m->free_size = m->free_size - sizeof(file_header);
        m->free_head = sizeof(mem_space) + sizeof(file_header);
    }

    else if(sizeof(file_header)>m->free_size)
    {
        file_header *fh = (file_header*)(temp + m->free_head);

        int f_n_len = strlen(file_name);
        int flag = 1;
        int len_n,len_ext;
        len_n = 0;
        while(flag && len_n<f_n_len)
        {
            if(file_name[len_n]=='.')
            {
                flag = 0;
            }
            else
            {
                len_n++;
            }
            
        }
        char *name = (char*)malloc((len_n+1)*sizeof(char));
        int co;
        for(co = 0;co<len_n;co++)
        {    
            name[co] = file_name[co];       
        }
        name[len_n] = '\0';
        len_ext = f_n_len - len_n;
        char *ext = (char*)malloc(len_ext*sizeof(char));
        for(co = co+1;co<f_n_len;co++)
        {
            ext[co - len_n - 1] = file_name[co];
        }
        ext[len_ext - 1] = '\0';

        fh->file_id = name;
        fh->file_type = ext;

        file_header *trav = (file_header*)(temp + m->files);
        int off = m->files;
        while(trav->next!=-1)
        {
            off = trav->next;
            trav = (file_header*)(temp + trav->next);
        }
        trav->next = m->free_head;
        fh->prev = off;
        fh->next = -1;
        fh->start_offset = -1;
        fh->end_offset = -1;

        m->free_size = m->free_size - sizeof(file_header);
        m->free_head = m->free_head + sizeof(file_header);


    }
    
    
}

