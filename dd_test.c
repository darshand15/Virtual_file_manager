#include "headers.h"

int file_exists(const char* file_name)
{
    FILE *fp = fopen(file_name,"r");
    if(fp==NULL)
    {
        return 0;
    }
    else
    {
        fclose(fp);
        return 1;
    }
    
}

void init_space(char **temp, int n)
{
    *temp = (char*)calloc(sizeof(char),n); //page allocation

    // if(temp==NULL)
    // {
    //     printf("No more space available\n");
    //     return;
    // }
    // mem_space *m = (mem_space*)temp;
    // m->free_size = n - sizeof(mem_space) - sizeof(book_keeper);
    // m->files = -1;
    
    // book_keeper *bk = (book_keeper*)(temp + sizeof(mem_space));
    // bk->next = -1;
    // bk->prev = -1;
    // bk->size = m->free_size;
    // bk->alloc_f = 'f';

}

void init_manager()
{
    char *fname = "file_manager.dat";
    int page_size = 1000;
    if(file_exists(fname)==0)
    {
        char *temp;
        init_space(&temp,page_size);
        FILE *fp = fopen(fname,"wb+");
        fseek(fp,0,SEEK_SET);
        fwrite(temp,1,page_size,fp);
        fclose(fp);
        fp = fopen(fname,"rb+");
        mem_space m;
        m.free_size = page_size - sizeof(mem_space) - sizeof(book_keeper);
        m.files = -1;
        fseek(fp,0,SEEK_SET);
        fwrite(&m,sizeof(mem_space),1,fp);

        book_keeper bk;
        bk.next = -1;
        bk.prev = -1;
        bk.size = m.free_size;
        bk.alloc_f = 'f';
        fseek(fp,sizeof(mem_space),SEEK_SET);
        fwrite(&bk,sizeof(book_keeper),1,fp);
        fclose(fp);

    }
    
}

int best_fit(FILE *fp,int req_size)
{

    mem_space m;
    fseek(fp,0,SEEK_SET);
    fread(&m,sizeof(mem_space),1,fp);

    book_keeper trav;
    book_keeper bk1,bk2;
    
    int flag_wbk = 0;
    int flag_wobk = 0;
    int min_size_wbk = INT_MAX;
    int min_size_wobk = INT_MAX;
    int off = sizeof(mem_space);
    int off_wbk, off_wobk;

    while(off!=-1)
    {
        fseek(fp,off,SEEK_SET);
        fread(&trav,sizeof(book_keeper),1,fp);
       
        if(trav.alloc_f == 'f' && trav.size > (req_size + sizeof(book_keeper)) && trav.size < min_size_wbk)
        {
            flag_wbk = 1;
            flag_wobk = 0;
            min_size_wbk = trav.size;
            bk1 = trav;
            off_wbk = off;
        }
        else if(flag_wbk==0 && trav.alloc_f == 'f' && trav.size>=req_size && trav.size<(req_size + sizeof(book_keeper)) && trav.size<min_size_wobk)
        {
            flag_wobk = 1;
            min_size_wobk = trav.size;
            bk2 = trav;
            off_wobk = off;

        }
        off = trav.next;
    }
    
    if(flag_wbk==1 && flag_wobk==0)
    {
        int init_size = bk1.size;
        bk1.alloc_f = 'a';
        bk1.size = req_size;

        book_keeper bk3;

        //book_keeper *bk3 = (book_keeper*)(temp + off_wbk + sizeof(book_keeper) + req_size);
        bk3.next = bk1.next;
        bk3.prev = off_wbk;
        if(bk1.next!=-1)
        {
            //book_keeper *bk4 = (book_keeper*)(temp + bk1->next);
            book_keeper bk4;
            fseek(fp,bk1.next,SEEK_SET);
            fread(&bk4,sizeof(book_keeper),1,fp);
            bk4.prev = off_wbk + sizeof(book_keeper) + req_size;
            fseek(fp,bk1.next,SEEK_SET);
            fwrite(&bk4,sizeof(book_keeper),1,fp);
        }
        bk1.next = off_wbk + sizeof(book_keeper) + req_size;
        bk3.alloc_f = 'f';
        bk3.size = init_size - req_size - sizeof(book_keeper); 

        m.free_size = m.free_size - req_size - sizeof(book_keeper);
        fseek(fp,0,SEEK_SET);
        fwrite(&m,sizeof(mem_space),1,fp);

        fseek(fp,off_wbk,SEEK_SET);
        fwrite(&bk1,sizeof(book_keeper),1,fp);

        fseek(fp,off_wbk + sizeof(book_keeper) + req_size,SEEK_SET);
        fwrite(&bk3,sizeof(book_keeper),1,fp);

        return (off_wbk + sizeof(book_keeper));

    }
    else if(flag_wobk==1 && flag_wbk==0)
    {        
        bk2.alloc_f = 'a';
        fseek(fp,off_wobk,SEEK_SET);
        fwrite(&bk2,sizeof(book_keeper),1,fp);

        m.free_size = m.free_size - bk2.size;
        fseek(fp,0,SEEK_SET);
        fwrite(&m,sizeof(mem_space),1,fp);

        return (off_wobk + sizeof(book_keeper));
    }
    else if(flag_wobk==0 && flag_wbk==0)
    {
        return -1;
    }

    return -1;

}


void create_file(const char* file_name)
{
    FILE *fp = fopen("file_manager.dat","rb+");
    mem_space m;
    fseek(fp,0,SEEK_SET);
    fread(&m,sizeof(mem_space),1,fp);

    if(m.files==-1 && sizeof(file_header)<(m.free_size))
    {
        int res = best_fit(fp,sizeof(file_header));
        if(res==-1)
        {
            printf("no space available\n");
            return;
        }

        m.files = res;
        //file_header *fh = (file_header*)(temp + m->files);
        file_header fh;


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

        //fh.file_id = name;
        strcpy(fh.file_id,name);
        fh.start_offset = -1;
        //fh.file_type = ext;
        strcpy(fh.file_type,ext);
        fh.end_offset = -1;
        fh.next = -1;
        fh.prev = -1;

        fseek(fp,0,SEEK_SET);
        fwrite(&m,sizeof(mem_space),1,fp);

        fseek(fp,res,SEEK_SET);
        fwrite(&fh,sizeof(file_header),1,fp);

    }

    else if(sizeof(file_header)<m.free_size)
    {
        int res = best_fit(fp,sizeof(file_header));
        if(res==-1)
        {
            printf("no space available\n");
            return;
        }

        //file_header *fh = (file_header*)(temp + res);
        file_header fh;

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

        //fh.file_id = name;
        strcpy(fh.file_id,name);
        //fh.file_type = ext;
        strcpy(fh.file_type,ext);

        //file_header *trav = (file_header*)(temp + m->files);
        file_header trav;
        fseek(fp,m.files,SEEK_SET);
        fread(&trav,sizeof(file_header),1,fp);

        int off = m.files;

        while(trav.next!=-1)
        {
            off = trav.next;
            //trav = (file_header*)(temp + trav->next);
            fseek(fp,off,SEEK_SET);
            fread(&trav,sizeof(file_header),1,fp);
        }
        trav.next = res;
        fh.prev = off;
        fh.next = -1;
        fh.start_offset = -1;
        fh.end_offset = -1;

        fseek(fp,off,SEEK_SET);
        fwrite(&trav,sizeof(file_header),1,fp);

        fseek(fp,res,SEEK_SET);
        fwrite(&fh,sizeof(file_header),1,fp);

    }

    fclose(fp);
     
}

void print_file_structure()
{
    FILE *fp = fopen("file_manager.dat","rb");
    mem_space m;
    fseek(fp,0,SEEK_SET);
    fread(&m,sizeof(mem_space),1,fp);
    //printf("%d\n",m.files);
    
    if(m.files==-1)
    {
        printf("no files present\n");
        return;
    }

    file_header trav;
    fseek(fp,m.files,SEEK_SET);
    fread(&trav,sizeof(file_header),1,fp);
    //printf("%d\n",trav.end_offset);


    while(trav.next!=-1)
    {
        //printf("test2\n");

        printf("file name: %s, file type: %s \n",trav.file_id,trav.file_type);
        fseek(fp,trav.next,SEEK_SET);
        fread(&trav,sizeof(file_header),1,fp);
    }

    printf("file name: %s, file type: %s \n",trav.file_id,trav.file_type);
    fclose(fp);

}

