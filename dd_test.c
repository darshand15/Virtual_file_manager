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
    int page_size = 332;
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

int best_fit(int req_size)
{
    printf("%d\n",req_size);
    FILE *fp = fopen("file_manager.dat","rb+");
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

        //finding the best fit block that can accomdate both the content and another book keeper
        if(trav.alloc_f == 'f' && trav.size > (req_size + sizeof(book_keeper)) && trav.size < min_size_wbk)
        {
            flag_wbk = 1;
            flag_wobk = 0;
            min_size_wbk = trav.size;
            bk1 = trav;
            off_wbk = off;
        }
        //finding the best fit block that can only accomodate the content but not another book keeper
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
        //best fit block that accomodates both the content and another book keeper

        int init_size = bk1.size;
        bk1.alloc_f = 'a';
        bk1.size = req_size;

        book_keeper bk3;

        bk3.next = bk1.next;
        bk3.prev = off_wbk;
        if(bk1.next!=-1)
        {
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

        fclose(fp);
        return (off_wbk + sizeof(book_keeper));

    }
    else if(flag_wobk==1 && flag_wbk==0)
    {        
        //best fit block that only accomodates the content
        //the remaining bytes are left in this block itself

        bk2.alloc_f = 'a';
        fseek(fp,off_wobk,SEEK_SET);
        fwrite(&bk2,sizeof(book_keeper),1,fp);

        m.free_size = m.free_size - bk2.size;
        fseek(fp,0,SEEK_SET);
        fwrite(&m,sizeof(mem_space),1,fp);

        fclose(fp);
        return (off_wobk + sizeof(book_keeper));
    }
    else if(flag_wobk==0 && flag_wbk==0)
    {
        //no block that can accomodate the required data is available
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return -1;

}

int largest_available_block()
{
    FILE *fp = fopen("file_manager.dat","rb+");
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

    fclose(fp);

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
        int res = best_fit(sizeof(file_header));
        if(res==-1)
        {
            printf("no space available\n");
            fclose(fp);
            return;
        }

        m.files = res;
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

        strcpy(fh.file_id,name);
        fh.start_offset = -1;
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
        int res = best_fit(sizeof(file_header));
        if(res==-1)
        {
            printf("no space available\n");
            fclose(fp);
            return;
        }

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

        strcpy(fh.file_id,name);
        strcpy(fh.file_type,ext);

        file_header trav;
        fseek(fp,m.files,SEEK_SET);
        fread(&trav,sizeof(file_header),1,fp);

        int off = m.files;

        while(trav.next!=-1)
        {
            off = trav.next;
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

    int res = best_fit(req_size + sizeof(file));

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

        

        int flag2 = 0; //flag to know if the required file header is found
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
           //previously allocated file blocks of this file are deleted and freed 
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
           fclose(fp);
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
                fclose(fp);
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

                fclose(fp);

            }
            
        }

    }

    /* if a block that can accomodate the entire content is not found, then the content
    is divided into chunks and allocated in different file blocks
    */
    else
    {
        printf("test1\n");
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

            /*flag to know if the content remaining after the first allocation to 
            the largest available block can be allocated directly using best fit algo
            or not
            */
            int flag3 = 0;
            int fprev;

            printf("%d\n",n);

            while(n!=0 && n>0)
            {
                printf("test2\n");
                large_b_off = largest_available_block();
                printf("%d %d\n",large_b_off,n);
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

                fclose(fp);
                fp = fopen("file_manager.dat","rb+");



                res2 = best_fit(n + sizeof(file));
                printf("%d\n",res2);

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
                    printf("test3\n");
                    fseek(fp,res2,SEEK_SET);
                    fwrite(&f3,sizeof(file),1,fp);
                    printf("test4\n");

                    end = old_n + i;
                    printf("%d %d",end,i);

                    char *content_3 = (char*)malloc((old_n + 1)*sizeof(char));
                    int j2 = 0;

                    while(i<end)
                    {
                        printf("test5\n");
                        content_3[j2] = content[i];
                        i++;
                        j2++;
                    }

                    content_3[end] = '\0';
                    printf("%s",content_3);

                    fseek(fp,res2 + sizeof(file),SEEK_SET);
                    fwrite(content_3,old_n,1,fp);
                    printf("test6\n");

                    file f4;
                    fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                    fread(&f4,sizeof(file),1,fp);
                    printf("test7\n");
                    f4.next = res2;
                    fseek(fp,large_b_off + sizeof(book_keeper),SEEK_SET);
                    fwrite(&f4,sizeof(file),1,fp);
                    printf("test8\n");

                    fh.end_offset = f3.end_of_block;
                    fseek(fp,tar_off,SEEK_SET);
                    fwrite(&fh,sizeof(file_header),1,fp);
                    printf("test9\n");

                }

                fclose(fp);
                fp = fopen("file_manager.dat","rb+");

            }
            fclose(fp);
            return;
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

                while(n!=0 && n>0)
                {
                    large_b_off = largest_available_block();
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

                    fclose(fp);
                    fp = fopen("file_manager.dat","rb+");

                    res2 = best_fit(n + sizeof(file));

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

                    fclose(fp);
                    fp = fopen("file_manager.dat","rb+");
                    

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

                while(n!=0 && n>0)
                {
                    large_b_off = largest_available_block();
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

                    fclose(fp);
                    fp = fopen("file_manager.dat","rb+");

                    res2 = best_fit(n + sizeof(file));

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

                fclose(fp);
                fp = fopen("file_manager.dat","rb+");
            }

            fclose(fp);
            
            return;
            
        }

    }
    
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
            fclose(fp);
            return 0;
        }

        int off_f = fh.start_offset;
        book_keeper trav,trav_n,trav_p;
        file f;
        

        while(off_f!=-1)
        {
            fseek(fp,off_f - sizeof(book_keeper),SEEK_SET);
            fread(&trav,sizeof(book_keeper),1,fp);
            m.free_size = m.free_size + trav.size;

            if(trav.next!=-1)
            {
                fseek(fp,trav.next,SEEK_SET);
                fread(&trav_n,sizeof(book_keeper),1,fp);
                
                if(trav_n.alloc_f=='f')
                {
                    trav.size = trav.size + trav_n.size + sizeof(book_keeper);
                    trav.next = trav_n.next;
                    if(trav_n.next!=-1)
                    {
                        book_keeper trav_nn;
                        fseek(fp,trav_n.next,SEEK_SET);
                        fread(&trav_nn,sizeof(book_keeper),1,fp);
                        trav_nn.prev = off_f - sizeof(book_keeper);
                        fseek(fp,trav_n.next,SEEK_SET);
                        fwrite(&trav_nn,sizeof(book_keeper),1,fp);
                    }

                }
            }
            trav.alloc_f = 'f';
            
            fseek(fp,off_f - sizeof(book_keeper),SEEK_SET);
            fwrite(&trav,sizeof(book_keeper),1,fp);

            if(trav.prev!=-1)
            {
                fseek(fp,trav.prev,SEEK_SET);
                fread(&trav_p,sizeof(book_keeper),1,fp);

                if(trav_p.alloc_f=='f')
                {
                    trav_p.size = trav_p.size + trav.size + sizeof(book_keeper);
                    trav_p.next = trav.next;
                    if(trav.next!=-1)
                    {
                        book_keeper trav_pn;
                        fseek(fp,trav.next,SEEK_SET);
                        fread(&trav_pn,sizeof(book_keeper),1,fp);
                        trav_pn.prev = trav.prev;
                        fseek(fp,trav.next,SEEK_SET);
                        fwrite(&trav_pn,sizeof(book_keeper),1,fp);
                    }

                    fseek(fp,trav.prev,SEEK_SET);
                    fwrite(&trav_p,sizeof(book_keeper),1,fp);
                }
            }

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
            book_keeper trav,trav_n,trav_p;
            file f;

            while(off_f!=-1)
            {
                fseek(fp,off_f - sizeof(book_keeper),SEEK_SET);
                fread(&trav,sizeof(book_keeper),1,fp);
                m.free_size = m.free_size + trav.size;

                if(trav.next!=-1)
                {
                    fseek(fp,trav.next,SEEK_SET);
                    fread(&trav_n,sizeof(book_keeper),1,fp);
                    
                    if(trav_n.alloc_f=='f')
                    {
                        trav.size = trav.size + trav_n.size + sizeof(book_keeper);
                        trav.next = trav_n.next;
                        if(trav_n.next!=-1)
                        {
                            book_keeper trav_nn;
                            fseek(fp,trav_n.next,SEEK_SET);
                            fread(&trav_nn,sizeof(book_keeper),1,fp);
                            trav_nn.prev = off_f - sizeof(book_keeper);
                            fseek(fp,trav_n.next,SEEK_SET);
                            fwrite(&trav_nn,sizeof(book_keeper),1,fp);
                        }

                    }
                }
                trav.alloc_f = 'f';
                
                fseek(fp,off_f - sizeof(book_keeper),SEEK_SET);
                fwrite(&trav,sizeof(book_keeper),1,fp);

                if(trav.prev!=-1)
                {
                    fseek(fp,trav.prev,SEEK_SET);
                    fread(&trav_p,sizeof(book_keeper),1,fp);

                    if(trav_p.alloc_f=='f')
                    {
                        trav_p.size = trav_p.size + trav.size + sizeof(book_keeper);
                        trav_p.next = trav.next;
                        if(trav.next!=-1)
                        {
                            book_keeper trav_pn;
                            fseek(fp,trav.next,SEEK_SET);
                            fread(&trav_pn,sizeof(book_keeper),1,fp);
                            trav_pn.prev = trav.prev;
                            fseek(fp,trav.next,SEEK_SET);
                            fwrite(&trav_pn,sizeof(book_keeper),1,fp);
                        }

                        fseek(fp,trav.prev,SEEK_SET);
                        fwrite(&trav_p,sizeof(book_keeper),1,fp);
                    }
                }

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
        

        book_keeper b1,b1_n,b1_p;
        fseek(fp,tar_off - sizeof(book_keeper),SEEK_SET);
        fread(&b1,sizeof(book_keeper),1,fp);
        m.free_size = m.free_size + b1.size;

        if(b1.next!=-1)
        {
            fseek(fp,b1.next,SEEK_SET);
            fread(&b1_n,sizeof(book_keeper),1,fp);

            if(b1_n.alloc_f=='f')
            {
                b1.size = b1.size + b1_n.size + sizeof(book_keeper);
                b1.next = b1_n.next;
                if(b1_n.next!=-1)
                {
                    book_keeper b1_nn;
                    fseek(fp,b1_n.next,SEEK_SET);
                    fread(&b1_nn,sizeof(book_keeper),1,fp);
                    b1_nn.prev = tar_off - sizeof(book_keeper);
                    fseek(fp,b1_n.next,SEEK_SET);
                    fwrite(&b1_nn,sizeof(book_keeper),1,fp);
                }
            }
        }

        b1.alloc_f = 'f';
        fseek(fp,tar_off - sizeof(book_keeper),SEEK_SET);
        fwrite(&b1,sizeof(book_keeper),1,fp);

        if(b1.prev!=-1)
        {
            fseek(fp,b1.prev,SEEK_SET);
            fread(&b1_p,sizeof(book_keeper),1,fp);

            if(b1_p.alloc_f=='f')
            {
                b1_p.size = b1_p.size + b1.size + sizeof(book_keeper);
                b1_p.next = b1.next;
                if(b1.next!=-1)
                {
                    book_keeper b1_pn;
                    fseek(fp,b1.next,SEEK_SET);
                    fread(&b1_pn,sizeof(book_keeper),1,fp);
                    b1_pn.prev = b1.prev;
                    fseek(fp,b1.next,SEEK_SET);
                    fwrite(&b1_pn,sizeof(book_keeper),1,fp);
                }

                fseek(fp,b1.prev,SEEK_SET);
                fwrite(&b1_p,sizeof(book_keeper),1,fp);
            }
        }

        fseek(fp,0,SEEK_SET);
        fwrite(&m,sizeof(mem_space),1,fp);

    }
    fclose(fp);
    return 1;
}

void read_file(const char* file_name)
{
    FILE *fp = fopen("file_manager.dat","rb");
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
        printf("file doesn't exist\n");
        fclose(fp);
        return;
    } 

    int off_f = fh.start_offset;    
    file f;
    printf("\nFile content:\n");

    while(off_f!=-1)
    {
        fseek(fp,off_f,SEEK_SET);
        fread(&f,sizeof(file),1,fp);

        int size = f.end_of_block - (off_f + sizeof(file));
        char *content = (char*)malloc((size+1)*(sizeof(char)));
        content[size] = '\0';
        
        fseek(fp,off_f + sizeof(file),SEEK_SET);
        fread(content,size,1,fp);
        printf("%s\n",content);

        off_f = f.next;

    }

    fclose(fp);
    return;

}

void print_file_structure()
{
    FILE *fp = fopen("file_manager.dat","rb");
    mem_space m;
    fseek(fp,0,SEEK_SET);
    fread(&m,sizeof(mem_space),1,fp);
    
    if(m.files==-1)
    {
        printf("no files present\n");
        return;
    }

    file_header trav;
    fseek(fp,m.files,SEEK_SET);
    fread(&trav,sizeof(file_header),1,fp);

    while(trav.next!=-1)
    {
        printf("file name: %s, file type: %s \n",trav.file_id,trav.file_type);
        fseek(fp,trav.next,SEEK_SET);
        fread(&trav,sizeof(file_header),1,fp);
    }

    printf("file name: %s, file type: %s \n",trav.file_id,trav.file_type);
    fclose(fp);

}

void print_bk()
{
    FILE *fp = fopen("file_manager.dat","rb+");
    book_keeper trav;
    int off_bk = sizeof(mem_space);

    while(off_bk!=-1)
    {
        fseek(fp,off_bk,SEEK_SET);
        fread(&trav,sizeof(book_keeper),1,fp);

        printf("%c, %d\n",trav.alloc_f,trav.size);

        off_bk = trav.next;
    }

    fclose(fp);
    
}

