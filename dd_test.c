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

int largest_available_block(FILE *fp)
{
    // mem_space m;
    // fseek(fp,0,SEEK_SET);
    // fread(&m,sizeof(mem_space),1,fp);

    book_keeper trav;
    int off = sizeof(mem_space);
    int max_off;

    int max_size = INT_MIN;

    while(off!=-1)
    {
        fseek(fp,off,SEEK_SET);
        fread(&trav,sizeof(book_keeper),1,fp);
        if(trav.alloc_f == 'f' && trav.size>max_size)
        {
            max_size = trav.size;
            max_off = off;
        }
        off = trav.next;

    }

    book_keeper l;
    fseek(fp,max_off,SEEK_SET);
    fread(&l,sizeof(book_keeper),1,fp);

    l.alloc_f = 'a';
    
    fseek(fp,max_off,SEEK_SET);
    fwrite(&l,sizeof(book_keeper),1,fp);


    return max_off;
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

void insert_into_file(const char* file_name ,char* content ,char mode)
{
    FILE *fp = fopen("file_manager.dat","rb+");
    mem_space m;
    fseek(fp,0,SEEK_SET);
    fread(&m,sizeof(mem_space),1,fp);
    int req_size = strlen(content);

    if(m.free_size < (req_size + sizeof(file)))
    {
        printf("no space available\n");
        return;
    }

    if(m.files==-1)
    {
        return;
    }

    int res = best_fit(fp,req_size + sizeof(file));

    if(res!=-1)
    {
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

        

        int flag2 = 0;
        int off = m.files;
        int tar_off;

        while(flag2==0 && off!=-1)
        {
            fseek(fp,off,SEEK_SET);
            fread(&fh,sizeof(file_header),1,fp);
            
            if(strcmp(fh.file_id,name)==0 && strcmp(fh.file_type,ext)==0)
            {
                flag2 = 1;
                tar_off = off;
            }

            off = fh.next;
        }

        if(flag2==0)
        {
            return;
        }

        if(mode=='w')
        {
           int succ = del_file(file_name,1);

           file f;
           f.next = -1;
           f.prev = -1;
           f.end_of_block = res + sizeof(file) + req_size;

           fh.start_offset = res;
           fh.end_offset = f.end_of_block;
           
           fseek(fp,res,SEEK_SET);
           fwrite(&f,sizeof(file),1,fp);

           fseek(fp,res + sizeof(file),SEEK_SET);
           fwrite(content,req_size,1,fp);

           fseek(fp,tar_off,SEEK_SET);
           fwrite(&fh,sizeof(file_header),1,fp);
           return;
        }

        else if(mode=='a')
        {
            if(fh.start_offset==-1)
            {
                file f;
                f.next = -1;
                f.prev = -1;
                f.end_of_block = res + sizeof(file) + req_size;

                fh.start_offset = res;
                fh.end_offset = f.end_of_block;

                fseek(fp,res,SEEK_SET);
                fwrite(&f,sizeof(file),1,fp);

                fseek(fp,res + sizeof(file),SEEK_SET);
                fwrite(content,req_size,1,fp);

                fseek(fp,tar_off,SEEK_SET);
                fwrite(&fh,sizeof(file_header),1,fp);
                return;
            }

            else
            {
                file trav;
                int off_f = fh.start_offset;
                int off_f_p = 0;

                while(off_f!=-1)
                {
                    fseek(fp,off_f,SEEK_SET);
                    fread(&trav,sizeof(file),1,fp);
                    off_f_p = off_f;
                    off_f = trav.next;
                }

                file f;
                f.prev = off_f_p;
                f.next = -1;
                f.end_of_block = res + sizeof(file) + req_size;
                fseek(fp,res,SEEK_SET);
                fwrite(&f,sizeof(file),1,fp);
                fseek(fp,res + sizeof(file),SEEK_SET);
                fwrite(content,req_size,1,fp);

                trav.next = res;
                fseek(fp,off_f_p,SEEK_SET);
                fwrite(&trav,sizeof(file),1,fp);

                fh.end_offset = f.end_of_block;
                fseek(fp,tar_off,SEEK_SET);
                fwrite(&fh,sizeof(file_header),1,fp);



            }
            
        }

    }

    /* if a block that can accomodate the entire content is not found, then the content
    is divided into chunks and allocated in different file blocks
    */

   else
   {
    //    int large_b_off;
    //    int n = strlen(content);
    //    book_keeper trav;

    //    while(n!=0)
    //    {
    //        large_b_off = largest_available_block(fp);
    //        fseek(fp,large_b_off,SEEK_SET);
    //        fread(&trav,sizeof(book_keeper),1,fp);

    //        file f1;



    //    }

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

        

        int flag2 = 0;
        int off = m.files;
        int tar_off;

        while(flag2==0 && off!=-1)
        {
            fseek(fp,off,SEEK_SET);
            fread(&fh,sizeof(file_header),1,fp);
            
            if(strcmp(fh.file_id,name)==0 && strcmp(fh.file_type,ext)==0)
            {
                flag2 = 1;
                tar_off = off;
            }

            off = fh.next;
        }

        if(flag2==0)
        {
            return;
        }

        fseek(fp,tar_off,SEEK_SET);
        fread(&fh,sizeof(file_header),1,fp);

        if(mode=='w')
        {
            int succ = del_file(file_name,1);
            
            int large_b_off,res2,l_b_off_2;
            int n = strlen(content);
            int old_n;
            book_keeper trav;
            int i = 0;
            int end;
            int flag3 = 0;
            int fprev;

            while(n!=0)
            {
                large_b_off = largest_available_block(fp);
                fseek(fp,large_b_off,SEEK_SET);
                fread(&trav,sizeof(book_keeper),1,fp);

                file f1;

                if(flag3==1)
                {
                    file f2;
                    fseek(fp,fprev,SEEK_SET);
                    fread(&f2,sizeof(file),1,fp);
                    f2.next = large_b_off + sizeof(book_keeper);
                    fseek(fp,fprev,SEEK_SET);
                    fwrite(&f2,sizeof(file),1,fp);
                    
                }
                
                if(n==req_size)
                {
                    f1.prev = -1;
                    fh.start_offset = large_b_off + sizeof(book_keeper);
                }
                else
                {
                    f1.prev = fprev;
                }
                
                f1.end_of_block = large_b_off + sizeof(book_keeper) +  trav.size;
                f1.next = -1;

                old_n = n;
                n = n - (trav.size - sizeof(file));
                end = (trav.size - sizeof(file)) + i;

                char *content_2 = (char*)malloc((trav.size - sizeof(file)+1)*sizeof(char));
                int j = 0;

                while(i<end)
                {
                    content_2[j] = content[i];
                    i++;
                    j++;
                }
                content_2[end] = '\0';

                fseek(fp,large_b_off + sizeof(book_keeper) + sizeof(file),SEEK_SET);
                fwrite(content_2,trav.size - sizeof(file),1,fp);

                fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                fwrite(&f1,sizeof(file),1,fp);



                res2 = best_fit(fp,n + sizeof(file));

                if(res2==-1)
                {
                    flag3 = 1;
                    fprev = large_b_off + sizeof(book_keeper);
                     
                }
                else
                {
                    flag3 = 0;
                    old_n = n;
                    n = 0;

                    file f3;
                    f3.next = -1;
                    f3.prev = large_b_off + sizeof(book_keeper);
                    f3.end_of_block = res2 + sizeof(file) + old_n;
                    fseek(fp,res2,SEEK_SET);
                    fwrite(&f3,sizeof(file),1,fp);

                    end = old_n + i;

                    char *content_3 = (char*)malloc((old_n + 1)*sizeof(char));
                    int j2 = 0;

                    while(i<end)
                    {
                        content_3[j2] = content[i];
                        i++;
                        j2++;
                    }

                    content_3[end] = '\0';

                    fseek(fp,res2 + sizeof(file),SEEK_SET);
                    fwrite(content_3,old_n,1,fp);

                    file f4;
                    fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                    fread(&f4,sizeof(file),1,fp);
                    f4.next = res2;
                    fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                    fwrite(&f4,sizeof(file),1,fp);

                    fh.end_offset = f3.end_of_block;
                    fseek(fp,tar_off,SEEK_SET);
                    fwrite(&fh,sizeof(file_header),1,fp);

                }
                

            }
        }

        else if(mode=='a')
        {
            if(fh.start_offset==-1)
            {
                int large_b_off,res2,l_b_off_2;
                int n = strlen(content);
                int old_n;
                book_keeper trav;
                int i = 0;
                int end;
                int flag3  = 0;
                int fprev;

                while(n!=0)
                {
                    large_b_off = largest_available_block(fp);
                    fseek(fp,large_b_off,SEEK_SET);
                    fread(&trav,sizeof(book_keeper),1,fp);

                    file f1;

                    if(flag3==1)
                    {
                        file f2;
                        fseek(fp,fprev,SEEK_SET);
                        fread(&f2,sizeof(file),1,fp);
                        f2.next = large_b_off + sizeof(book_keeper);
                        fseek(fp,fprev,SEEK_SET);
                        fwrite(&f2,sizeof(file),1,fp);
                        
                    }
                    
                    if(n==req_size)
                    {
                        f1.prev = -1;
                        fh.start_offset = large_b_off + sizeof(book_keeper);
                    }
                    else
                    {
                        f1.prev = fprev;
                    }
                    
                    f1.end_of_block = large_b_off + sizeof(book_keeper) +  trav.size;
                    f1.next = -1;

                    old_n = n;
                    n = n - (trav.size - sizeof(file));
                    end = (trav.size - sizeof(file)) + i;

                    char *content_2 = (char*)malloc((trav.size - sizeof(file)+1)*sizeof(char));
                    int j = 0;

                    while(i<end)
                    {
                        content_2[j] = content[i];
                        i++;
                        j++;
                    }
                    content_2[end] = '\0';

                    fseek(fp,large_b_off + sizeof(book_keeper) + sizeof(file),SEEK_SET);
                    fwrite(content_2,trav.size - sizeof(file),1,fp);

                    fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                    fwrite(&f1,sizeof(file),1,fp);



                    res2 = best_fit(fp,n + sizeof(file));

                    if(res2==-1)
                    {
                        flag3 = 1;
                        fprev = large_b_off + sizeof(book_keeper);
                        
                    }
                    else
                    {
                        flag3 = 0;
                        old_n = n;
                        n = 0;

                        file f3;
                        f3.next = -1;
                        f3.prev = large_b_off + sizeof(book_keeper);
                        f3.end_of_block = res2 + sizeof(file) + old_n;
                        fseek(fp,res2,SEEK_SET);
                        fwrite(&f3,sizeof(file),1,fp);

                        end = old_n + i;

                        char *content_3 = (char*)malloc((old_n + 1)*sizeof(char));
                        int j2 = 0;

                        while(i<end)
                        {
                            content_3[j2] = content[i];
                            i++;
                            j2++;
                        }

                        content_3[end] = '\0';

                        fseek(fp,res2 + sizeof(file),SEEK_SET);
                        fwrite(content_3,old_n,1,fp);

                        file f4;
                        fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                        fread(&f4,sizeof(file),1,fp);
                        f4.next = res2;
                        fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                        fwrite(&f4,sizeof(file),1,fp);

                        fh.end_offset = f3.end_of_block;
                        fseek(fp,tar_off,SEEK_SET);
                        fwrite(&fh,sizeof(file_header),1,fp);

                    }
                    

                }
            }

            else
            {
                int large_b_off,res2,l_b_off_2;
                int n = strlen(content);
                int old_n;
                book_keeper trav;
                int i = 0;
                int end;
                int flag3 = 0;
                int fprev;

                file trav_f;
                int off_f = fh.start_offset;
                int off_f_p = 0;

                while(off_f!=-1)
                {
                    fseek(fp,off_f,SEEK_SET);
                    fread(&trav_f,sizeof(file),1,fp);
                    off_f_p = off_f;
                    off_f = trav_f.next;
                }

                while(n!=0)
                {
                    large_b_off = largest_available_block(fp);
                    fseek(fp,large_b_off,SEEK_SET);
                    fread(&trav,sizeof(book_keeper),1,fp);

                    file f1;

                    if(flag3==1)
                    {
                        file f2;
                        fseek(fp,fprev,SEEK_SET);
                        fread(&f2,sizeof(file),1,fp);
                        f2.next = large_b_off + sizeof(book_keeper);
                        fseek(fp,fprev,SEEK_SET);
                        fwrite(&f2,sizeof(file),1,fp);
                        
                    }
                    
                    if(n==req_size)
                    {
                        f1.prev = off_f_p;
                        trav_f.next = large_b_off + sizeof(book_keeper);
                        fseek(fp,off_f_p,SEEK_SET);
                        fwrite(&trav_f,sizeof(file),1,fp);
                    }
                    else
                    {
                        f1.prev = fprev;
                    }
                    
                    f1.end_of_block = large_b_off + sizeof(book_keeper) +  trav.size;
                    f1.next = -1;

                    old_n = n;
                    n = n - (trav.size - sizeof(file));
                    end = (trav.size - sizeof(file)) + i;

                    char *content_2 = (char*)malloc((trav.size - sizeof(file)+1)*sizeof(char));
                    int j = 0;

                    while(i<end)
                    {
                        content_2[j] = content[i];
                        i++;
                        j++;
                    }
                    content_2[end] = '\0';

                    fseek(fp,large_b_off + sizeof(book_keeper) + sizeof(file),SEEK_SET);
                    fwrite(content_2,trav.size - sizeof(file),1,fp);

                    fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                    fwrite(&f1,sizeof(file),1,fp);

                    res2 = best_fit(fp,n + sizeof(file));

                    if(res2==-1)
                    {
                        flag3 = 1;
                        fprev = large_b_off + sizeof(book_keeper);
                        
                    }
                    else
                    {
                        flag3 = 0;
                        old_n = n;
                        n = 0;

                        file f3;
                        f3.next = -1;
                        f3.prev = large_b_off + sizeof(book_keeper);
                        f3.end_of_block = res2 + sizeof(file) + old_n;
                        fseek(fp,res2,SEEK_SET);
                        fwrite(&f3,sizeof(file),1,fp);

                        end = old_n + i;

                        char *content_3 = (char*)malloc((old_n + 1)*sizeof(char));
                        int j2 = 0;

                        while(i<end)
                        {
                            content_3[j2] = content[i];
                            i++;
                            j2++;
                        }

                        content_3[end] = '\0';

                        fseek(fp,res2 + sizeof(file),SEEK_SET);
                        fwrite(content_3,old_n,1,fp);

                        file f4;
                        fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                        fread(&f4,sizeof(file),1,fp);
                        f4.next = res2;
                        fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                        fwrite(&f4,sizeof(file),1,fp);

                        fh.end_offset = f3.end_of_block;
                        fseek(fp,tar_off,SEEK_SET);
                        fwrite(&fh,sizeof(file_header),1,fp);

                    }
                    

                }
            }
            
        }


   }

    fclose(fp);

}

/*del_mode has the value 0 if everything including the file contents and 
the file header has to be deleted
del_mode has the value 1 if only the file contents has to be deleted 
and the file header isn't
*/
int del_file(const char* file_name,int del_mode)
{
    FILE *fp = fopen("file_manager.dat","rb+");
    mem_space m;
    fseek(fp,0,SEEK_SET);
    fread(&m,sizeof(mem_space),1,fp);

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
    
    int flag2 = 0;
    int off = m.files;
    int tar_off;

    while(flag2==0 && off!=-1)
    {
        fseek(fp,off,SEEK_SET);
        fread(&fh,sizeof(file_header),1,fp);
        
        if(strcmp(fh.file_id,name)==0 && strcmp(fh.file_type,ext)==0)
        {
            flag2 = 1;
            tar_off = off;
        }

        off = fh.next;
    }

    if(flag2==0)
    {
        //file doesn't exist
        return -1;
    } 

    

    if(del_mode==1)
    {
        if(fh.start_offset==-1)
        {
            //file has no contents
            return 0;
        }

        int off_f = fh.start_offset;
        book_keeper trav;
        file f;

        while(off_f!=-1)
        {
            fseek(fp,off_f - sizeof(book_keeper),SEEK_SET);
            fread(&trav,sizeof(book_keeper),1,fp);
            trav.alloc_f = 'f';
            m.free_size = m.free_size + trav.size;
            fseek(fp,off_f - sizeof(book_keeper),SEEK_SET);
            fwrite(&trav,sizeof(book_keeper),1,fp);

            fseek(fp,off_f,SEEK_SET);
            fread(&f,sizeof(file),1,fp);
            off_f = f.next;
        }

        fseek(fp,0,SEEK_SET);
        fwrite(&m,sizeof(mem_space),1,fp);

    }

    else if(del_mode==0)
    {
        if(fh.start_offset!=-1)
        {
            int off_f = fh.start_offset;
            book_keeper trav;
            file f;

            while(off_f!=-1)
            {
                fseek(fp,off_f - sizeof(book_keeper),SEEK_SET);
                fread(&trav,sizeof(book_keeper),1,fp);
                trav.alloc_f = 'f';
                m.free_size = m.free_size + trav.size;
                fseek(fp,off_f - sizeof(book_keeper),SEEK_SET);
                fwrite(&trav,sizeof(book_keeper),1,fp);

                fseek(fp,off_f,SEEK_SET);
                fread(&f,sizeof(file),1,fp);
                off_f = f.next;
            }
        }

        if(tar_off==m.files)
        {
            m.files = fh.next;
            file_header fh2;
            fseek(fp,fh.next,SEEK_SET);
            fread(&fh2,sizeof(file_header),1,fp);
            fh2.prev = -1;
            fseek(fp,fh.next,SEEK_SET);
            fwrite(&fh2,sizeof(file_header),1,fp);
        }

        else
        {
            file_header fh_prev, fh_next;
            fseek(fp,fh.prev,SEEK_SET);
            fread(&fh_prev,sizeof(file_header),1,fp);
            fh_prev.next = fh.next;
            fseek(fp,fh.prev,SEEK_SET);
            fwrite(&fh_prev,sizeof(file_header),1,fp);
            if(fh.next!=-1)
            {
                fseek(fp,fh.next,SEEK_SET);
                fread(&fh_next,sizeof(file_header),1,fp);
                fh_next.prev = fh.prev;
                fseek(fp,fh.next,SEEK_SET);
                fwrite(&fh_next,sizeof(file_header),1,fp);
            }
        }
        

        book_keeper b1;
        fseek(fp,tar_off - sizeof(book_keeper),SEEK_SET);
        fread(&b1,sizeof(book_keeper),1,fp);
        b1.alloc_f = 'f';
        fseek(fp,tar_off - sizeof(book_keeper),SEEK_SET);
        fwrite(&b1,sizeof(book_keeper),1,fp);

        m.free_size = m.free_size + b1.size;
        fseek(fp,0,SEEK_SET);
        fwrite(&m,sizeof(mem_space),1,fp);

    }
    return 1;
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

