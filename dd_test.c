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
    m->free_size = n - sizeof(mem_space) - sizeof(book_keeper);
    m->files = -1;
    m->free_head = -1;

    book_keeper *bk = (book_keeper*)(temp + sizeof(mem_space));
    bk->next = -1;
    bk->prev = -1;
    bk->size = m->free_size;
    bk->alloc_f = 'f';

}

void init_manager(void *temp)
{
    init_space(temp,1000);
}

int best_fit(void *temp,int req_size)
{
    mem_space *m = (mem_space*)temp;
    book_keeper *trav;
    book_keeper *bk1,*bk2;
    
    int flag_wbk = 0;
    int flag_wobk = 0;
    int min_size_wbk = INT_MAX;
    int min_size_wobk = INT_MAX;
    int off = sizeof(mem_space);
    int off_wbk, off_wobk;

    while(off!=-1)
    {
        trav = (book_keeper*)(temp + off);
       
        if(trav->alloc_f == 'f' && trav->size > (req_size + sizeof(book_keeper)) && trav->size < min_size_wbk)
        {
            flag_wbk = 1;
            flag_wobk = 0;
            min_size_wbk = trav->size;
            bk1 = trav;
            off_wbk = off;
        }
        else if(flag_wbk==0 && trav->alloc_f == 'f' && trav->size>=req_size && trav->size<(req_size + sizeof(book_keeper)) && trav->size<min_size_wobk)
        {
            flag_wobk = 1;
            min_size_wobk = trav->size;
            bk2 = trav;
            off_wobk = off;

        }
        off = trav->next;
    }
    
    if(flag_wbk==1 && flag_wobk==0)
    {
        int init_size = bk1->size;
        bk1->alloc_f = 'a';
        bk1->size = req_size;
        book_keeper *bk3 = (book_keeper*)(temp + off_wbk + sizeof(book_keeper) + req_size);
        bk3->next = bk1->next;
        bk3->prev = off_wbk;
        if(bk1->next!=-1)
        {
            book_keeper *bk4 = (book_keeper*)(temp + bk1->next);
            bk4->prev = off_wbk + sizeof(book_keeper) + req_size;
        }
        bk1->next = off_wbk + sizeof(book_keeper) + req_size;
        bk3->alloc_f = 'f';
        bk3->size = init_size - req_size - sizeof(book_keeper); 
        return (off_wbk + sizeof(book_keeper));

    }
    else if(flag_wobk==1 && flag_wbk==0)
    {        
        bk2->alloc_f = 'a';
        return (off_wobk + sizeof(book_keeper));
    }
    else if(flag_wobk==0 && flag_wbk==0)
    {
        return -1;
    }

}


void create_file(const char* file_name, void *temp)
{
    mem_space *m = (mem_space*)temp;
    if(m->files==-1 && sizeof(file_header)>(m->free_size))
    {
        m->files = sizeof(mem_space) + sizeof(book_keeper);
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

        book_keeper *bk1 = (book_keeper*)(temp + sizeof(mem_space));
        //book_keeper *bk2 = (book_keeper*)(temp + )
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

