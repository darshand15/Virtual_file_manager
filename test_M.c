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

void init_space(char** temp, int n)
{
    *temp = calloc(sizeof(char), n);
}
void init_manager()
{
    char* filename = "file_manager.dat";
    if(!exists(filename))
    {   
        char* temp;
        init_space(&temp, 1000);
        FILE* fp = fopen(filename,"wb+");
        fseek(fp, 0, SEEK_SET);
        fwrite(temp, 1, 1000, fp);
        fclose(fp);
        fp = fopen(filename,"rb+");
        mem_space m;
        m.size = 1000 - sizeof(mem_space) - sizeof(bk);
        m.file_offset = -1;
        fseek(fp, 0, SEEK_SET);
        fwrite(&m, sizeof(mem_space), 1, fp);
        
        bk b;
        b.next = -1;
        b.prev = -1;
        b.alloc = 0;
        b.size = m.size;
        fseek(fp, sizeof(mem_space), SEEK_SET);
        fwrite(&b, sizeof(bk), 1, fp);
        fclose(fp);
    }
    
}

int worst_block(FILE* fp, int r_size) 
{  
    mem_space head_mem_space;
    fseek(fp, 0, SEEK_SET);
    fread(&head_mem_space, sizeof(mem_space), 1, fp);
 
    bk index, max;
    int offset_cb = -1;
    int offset_index = sizeof(mem_space);
    int max_flag = 0;
    fseek(fp, sizeof(mem_space), SEEK_SET);
    fread(&index, sizeof(bk), 1, fp);

    if(index.next == -1)
    {   
        if(index.size >= r_size)
        {        
            return offset_index;
        }
        else
        {
            return -1;
        }
        
    }
    else
    {   
        if(index.alloc)
        {
            max.size = 0;
        }
        else
        {
            max = index;
            offset_cb = offset_index;
        }
        
        do
        {   
            offset_index = index.next;
            fseek(fp, index.next, SEEK_SET);
            fread(&index, sizeof(bk), 1, fp);
            if(!(index.alloc) && (index.size > max.size) )
            {
                max = index;
                offset_cb = offset_index;
                max_flag = 1;
            }


        } while(index.next != -1);
    }

    if(max_flag && max.size >= r_size)    
    {
        return offset_cb;
    } 
    else
    {
        return -1;
    }
}


int create_file(const char* filename, const char* filetype)
{
    FILE* fp = fopen("file_manager.dat", "rb+");
    mem_space m;
    fseek(fp, 0, SEEK_SET);
    fread(&m, sizeof(mem_space), 1, fp);
    int r_size = sizeof(file_header);

    int offset_cb = worst_block(fp, r_size);

    if(offset_cb == -1)
    {
        return -1;
    }
    else
    {   
        bk block;
        fseek(fp, offset_cb, SEEK_SET);
        fread(&block, sizeof(bk), 1, fp);

        if(block.size > (r_size + sizeof(bk)))
        {
            int offset_fb = offset_cb + sizeof(bk) + r_size;
            bk free_block;
            free_block.next = block.next;
            block.next = offset_fb;
            free_block.prev = offset_cb;
            free_block.size = block.size - sizeof(bk) - r_size;
            free_block.alloc = 0;
            block.size = r_size;
            m.size -= sizeof(bk);
            fseek(fp, offset_fb, SEEK_SET);
            fwrite(&free_block, sizeof(bk), 1, fp);
            fseek(fp, 0, SEEK_SET);
            fwrite(&m, sizeof(mem_space), 1, fp);
        }
        if(m.file_offset == -1)
        {
            m.file_offset = offset_cb + sizeof(bk);
            file_header file_h;
            file_h.next = -1;
            file_h.file_id = 0;
            strcpy(file_h.file_name, filename);
            strcpy(file_h.file_type, filetype);
            file_h.start_offset = -1;
            file_h.end_offset = -1;
            file_h.prev = -1;
            block.alloc = 1;

            fseek(fp, offset_cb, SEEK_SET);
            fwrite(&block, sizeof(bk), 1, fp);

            fseek(fp, m.file_offset, SEEK_SET);
            fwrite(&file_h, sizeof(file_header), 1, fp);

            fseek(fp, 0, SEEK_SET);
            fwrite(&m, sizeof(mem_space), 1, fp);
        }
        else
        {
            file_header file_h;
            int c_file_offset = offset_cb + sizeof(bk);
            file_h.file_id = 0;
            strcpy(file_h.file_name, filename);
            strcpy(file_h.file_type, filetype);
            file_h.start_offset = -1;
            file_h.end_offset = -1;
            file_header p_file;
            fseek(fp, m.file_offset, SEEK_SET);
            fread(&p_file, sizeof(file_header), 1, fp);

            int p_file_offset = m.file_offset;
            while(p_file.next != -1)
            {
                p_file_offset = p_file.next;
                fseek(fp, p_file_offset, SEEK_SET);
                fread(&p_file, sizeof(file_header), 1, fp);
            }
            p_file.next = c_file_offset;
            file_h.file_id = p_file.file_id + 1;
            file_h.prev = p_file_offset;
            file_h.next = -1;
            block.alloc = 1;

            fseek(fp, p_file_offset, SEEK_SET);
            fwrite(&p_file, sizeof(file_header), 1, fp);

            fseek(fp, c_file_offset, SEEK_SET);
            fwrite(&file_h, sizeof(file_header), 1, fp);
        }
        m.size -= (r_size);
        fseek(fp, 0, SEEK_SET);
        fwrite(&m, sizeof(mem_space), 1, fp);

        fseek(fp, offset_cb, SEEK_SET);
        fwrite(&block, sizeof(bk), 1, fp);
        fclose(fp);
        return 1;
    }
}

int check_file_exists(const char* filename, const char* filetype)
{
    FILE *fp = fopen("file_manager.dat","rb");
    mem_space m;
    fseek(fp,0,SEEK_SET);
    fread(&m,sizeof(mem_space),1,fp);
    if(m.file_offset == -1)
    {
        return 0;
    }
    file_header f_head;
    fseek(fp, m.file_offset, SEEK_SET);
    fread(&f_head, sizeof(file_header), 1, fp);
    int offset_f_h = m.file_offset;
    while(f_head.next != -1)
    {
        if(!strcmp(f_head.file_name, filename) && !strcmp(f_head.file_type, filetype))
        {
            return 1;
            break;
        }
        offset_f_h = f_head.next;
        fseek(fp, f_head.next, SEEK_SET);
        fread(&f_head, sizeof(file_header), 1, fp);
    }
    if(!strcmp(f_head.file_name, filename) && !strcmp(f_head.file_type, filetype))
    {
        return 1;
    }
    return 0;
}

void edit_file(const char* filename, const char* filetype, char* s, char mode)
{
    FILE *fp = fopen("file_manager.dat","rb+");
    mem_space m;
    fseek(fp,0,SEEK_SET);
    fread(&m,sizeof(mem_space),1,fp);
    int t_size = strlen(s);
    int r_size = t_size + sizeof(file);
    if(m.file_offset == -1)
    {
        printf("\nNo file has been created.\n");
        fclose(fp);
        return;
    }
    if(m.size > (r_size) || mode == 'w') 
    {
        file_header f_head;
        fseek(fp, m.file_offset, SEEK_SET);
        fread(&f_head, sizeof(file_header), 1, fp);
        int found = 0, offset_f_h = m.file_offset;
        while(f_head.next != -1)
        {
            if(!strcmp(f_head.file_name, filename) && !strcmp(f_head.file_type, filetype))
            {
                found = 1;
                break;
            }
            offset_f_h = f_head.next;
            fseek(fp, f_head.next, SEEK_SET);
            fread(&f_head, sizeof(file_header), 1, fp);
        }
        if(!strcmp(f_head.file_name, filename) && !strcmp(f_head.file_type, filetype))
        {
                found = 1;
        }
        if(found)
        {
            int offset_file = f_head.start_offset;
            switch (mode)
            {
            case 'r':
                while(offset_file != -1)
                {
                    file f;
                    fseek(fp, offset_file, SEEK_SET);
                    fread(&f, sizeof(file), 1, fp);
                    int t_size = f.end_of_block - offset_file - sizeof(file);
                    char* temp = malloc(sizeof(char)*(t_size+1));
                    temp[t_size] = '\0'; // JUST IN CASE
                    fseek(fp, offset_file + sizeof(file), SEEK_SET);
                    fread(temp, sizeof(char), t_size, fp);
                    printf("\n(FILE BLOCK) %s \n", temp);
                    offset_file = f.next;
                }
                printf("\n file contents printed\n");
                break;
            case 'w':
                fclose(fp);
                delete_all_fb(f_head);
                fp = fopen("file_manager.dat", "rb+");
                fseek(fp,0,SEEK_SET);
                fread(&m,sizeof(mem_space),1,fp);
                if(m.size < (r_size))
                {
                    printf("\nNo space available\n");
                    return;
                }
                f_head.start_offset = -1;
                file f;
                f.next = -1;
                f.prev = -1;
                int offset_cb = worst_block(fp, r_size);
                if(offset_cb != -1)
                {
                    bk block;
                    fseek(fp, offset_cb, SEEK_SET);
                    fread(&block, sizeof(bk), 1, fp);
                    if(block.prev == -1)
                    {
                        offset_cb = sizeof(mem_space);
                    }
                    else
                    {
                        bk prev_block;
                        fseek(fp, block.prev, SEEK_SET);
                        fread(&prev_block, sizeof(bk), 1, fp);
                        offset_cb = prev_block.next;
                    }
                    f.end_of_block = offset_cb + sizeof(bk) + r_size;
                    f_head.end_offset = f.end_of_block;
                    f_head.start_offset = offset_cb + sizeof(bk);

                    if(block.size > (r_size + sizeof(bk)))
                    {
                        int offset_fb = offset_cb + sizeof(bk) + r_size;
                        bk free_block;
                        free_block.next = block.next;
                        block.next = offset_fb;
                        free_block.prev = offset_cb;
                        free_block.size = block.size - sizeof(bk) - r_size;
                        free_block.alloc = 0;
                        block.size = r_size;
                        block.alloc = 1;
                        fseek(fp, offset_fb, SEEK_SET);
                        fwrite(&free_block, sizeof(bk), 1, fp);
                        fseek(fp, offset_cb, SEEK_SET);
                        fwrite(&block, sizeof(bk), 1, fp);
                    }
                    offset_file = f_head.start_offset;
                    fseek(fp, offset_f_h, SEEK_SET);
                    fwrite(&f_head, sizeof(file_header), 1, fp);

                    fseek(fp, offset_file, SEEK_SET);
                    fwrite(&f, sizeof(file), 1, fp);

                    fseek(fp, offset_file + sizeof(file), SEEK_SET);
                    fwrite(s, sizeof(char), t_size, fp);

                    fseek(fp, 0, SEEK_SET);
                    fread(&m, sizeof(mem_space), 1, fp);
                    m.size -= (r_size + sizeof(bk));
                    fseek(fp, 0, SEEK_SET);
                    fwrite(&m, sizeof(mem_space), 1, fp);
                }
                else if(size_of_free_blocks() > (t_size + no_of_free_blocks()*sizeof(file)))
                {   fclose(fp);

                    int offset_p_f = -1;
                    int o = 0;
                    while (t_size > 0)
                    {
                        fp = fopen("file_manager.dat", "rb+");
                        int offset_cb = worst_block(fp, 0);
                        bk block;
                        fseek(fp, offset_cb, SEEK_SET);
                        fread(&block, sizeof(bk), 1, fp);

                        int block_t_size = block.size - sizeof(file);
                        t_size -= block_t_size;
                        char* text = s + o;

                        file f;
                        f.next = -1;
                        if(offset_p_f != -1)
                        {
                            f.prev = offset_p_f;
                            file p_f;
                            fseek(fp, offset_p_f, SEEK_SET);
                            fread(&p_f, sizeof(file), 1, fp);

                            p_f.next = offset_cb + sizeof(bk);

                            fseek(fp, offset_p_f, SEEK_SET);
                            fwrite(&p_f, sizeof(file), 1, fp);
                        }
                        else
                        {
                            f.prev = -1;
                            f_head.start_offset = offset_cb + sizeof(bk);
                            fseek(fp, offset_f_h, SEEK_SET);
                            fwrite(&f_head, sizeof(file_header), 1, fp);
                        }
                        f.end_of_block = offset_cb + sizeof(bk) + sizeof(file) + block_t_size;

                        block.alloc = 1;
                        
                        fseek(fp, offset_cb, SEEK_SET);
                        fwrite(&block, sizeof(bk), 1, fp);

                        offset_p_f = offset_cb + sizeof(bk);

                        fseek(fp, offset_p_f, SEEK_SET);
                        fwrite(&f, sizeof(f), 1, fp);

                        fseek(fp, offset_p_f + sizeof(file), SEEK_SET);
                        fwrite(text, sizeof(char), block_t_size, fp);
                        o += block_t_size;

                        mem_space m;
                        fseek(fp, 0, SEEK_SET);
                        fread(&m, sizeof(mem_space), 1, fp);
                        m.size -= (block.size);
                        fseek(fp, 0, SEEK_SET);
                        fwrite(&m, sizeof(mem_space), 1, fp);
                        fclose(fp);
                    }
                    
                }
                else
                {
                    printf("\nNo Space Availabe\n");
                    fclose(fp);
                    return;
                }
                
                break;
            case 'a':
                if(offset_file == -1)
                {
                    edit_file(filename, filetype, s, 'w');
                }
                else
                {
                    int offset_p_f = 0;
                    while(offset_file != -1)
                    {
                        file f;
                        fseek(fp, offset_file, SEEK_SET);
                        fread(&f, sizeof(file), 1, fp);
                        offset_p_f = offset_file;
                        offset_file = f.next;
                    }
                    file p_f;
                    fseek(fp, offset_p_f, SEEK_SET);
                    fread(&p_f, sizeof(file), 1, fp);
                    int offset_cb = worst_block(fp, r_size);
                    if(offset_cb != -1)
                    {
                        bk block;
                        fseek(fp, offset_cb, SEEK_SET);
                        fread(&block, sizeof(bk), 1, fp);
                        file f;
                        f.next = -1;
                        f.prev = offset_p_f;
                        if(block.prev == -1)
                        {
                            offset_cb = sizeof(mem_space);
                        }
                        else
                        {
                            bk prev_block;
                            fseek(fp, block.prev, SEEK_SET);
                            fread(&prev_block, sizeof(bk), 1, fp);
                            offset_cb = prev_block.next;
                        }
                        offset_file = offset_cb + sizeof(bk);
                        f.end_of_block = offset_file + r_size;
                        f_head.end_offset = f.end_of_block;
                        p_f.next = offset_file;

                        if(block.size > (r_size + sizeof(bk)))
                        {
                            int offset_fb = offset_cb + sizeof(bk) + r_size;
                            bk free_block;
                            free_block.next = block.next;
                            block.next = offset_fb;
                            free_block.prev = offset_cb;
                            free_block.size = block.size - sizeof(bk) - r_size;
                            free_block.alloc = 0;
                            block.size = r_size;
                            fseek(fp, offset_fb, SEEK_SET);
                            fwrite(&free_block, sizeof(bk), 1, fp);
                            fseek(fp, offset_cb, SEEK_SET);
                            fwrite(&block, sizeof(bk), 1, fp);
                        }
                        block.alloc = 1;
                        fseek(fp, offset_cb, SEEK_SET);
                        fwrite(&block, sizeof(bk), 1, fp);
                        fseek(fp, offset_f_h, SEEK_SET);
                        fwrite(&f_head, sizeof(file_header), 1, fp);

                        fseek(fp, offset_p_f, SEEK_SET);
                        fwrite(&p_f, sizeof(file), 1, fp);

                        fseek(fp, offset_file, SEEK_SET);
                        fwrite(&f, sizeof(file), 1, fp);

                        fseek(fp, offset_file + sizeof(file), SEEK_SET);
                        fwrite(s, sizeof(char), t_size, fp);

                        m.size -= (r_size + sizeof(bk));
                        fseek(fp, 0, SEEK_SET);
                        fwrite(&m, sizeof(mem_space), 1, fp);
                    }
                    else if (size_of_free_blocks() > (t_size + no_of_free_blocks()*sizeof(file)))
                    {
                        while (t_size > 0)
                        {
                            int offset_cb = worst_block(fp, 0);
                            bk block;
                            fseek(fp, offset_cb, SEEK_SET);
                            fread(&block, sizeof(bk), 1, fp);
                            int block_t_size = block.size - sizeof(file);
                            t_size -= block_t_size;
                            char* text = malloc(sizeof(char)*(block_t_size+1));
                            strncpy(text, s, block_t_size);
                            text[block_t_size] = '\0';
                            edit_file(filename, filetype, text, 'a');
                            free(text);
                        }
                    }
                    else
                    {
                        printf("\nNo space availabe\n");
                        fclose(fp);
                        return;
                    }
                                       
                }
                break;
            default:break;
            }
        }
        else
        {
            printf("\nFile not found\n");
            fclose(fp);
            return;
        }
        
    }
    else
    {
        printf("\nSpace not available\n");
        fclose(fp);
        return;
    }
    fclose(fp);
}
void check_block_integrity()
{
    FILE* fp = fopen("file_manager.dat","rb");
    bk block;
    fseek(fp, sizeof(mem_space), SEEK_SET);
    fread(&block, sizeof(bk), 1, fp);

    int offset_cb = block.next;
    printf("\nCBI alloc:%d size:%d next:%d prev:%d\n", block.alloc, block.size, block.next, block.prev);
    while (offset_cb != -1)
    {   
        fseek(fp, offset_cb, SEEK_SET);
        fread(&block, sizeof(bk), 1, fp);
        printf("\nCBI alloc:%d size:%d next:%d prev:%d\n", block.alloc, block.size, block.next, block.prev);
        offset_cb = block.next;
    }
    fclose(fp);
    return;
}

void verify_free_bk()
{
    FILE* fp = fopen("file_manager.dat","rb");
    bk block;
    fseek(fp, sizeof(mem_space), SEEK_SET);
    fread(&block, sizeof(bk), 1, fp);

    int offset_cb = block.next;
    while (offset_cb != -1)
    {   
        fseek(fp, offset_cb, SEEK_SET);
        fread(&block, sizeof(bk), 1, fp);
        if(block.alloc == 0)free_bk(offset_cb);
        offset_cb = block.next;
    }
    fclose(fp);
    update_m();
    return;
}
int size_of_free_blocks()
{
    FILE* fp = fopen("file_manager.dat","rb");
    int size = 0;
    bk block;
    fseek(fp, sizeof(mem_space), SEEK_SET);
    fread(&block, sizeof(bk), 1, fp);

    int offset_cb = block.next;
    if(!block.alloc)  size+=block.size;
    while (offset_cb != -1)
    {
        fseek(fp, offset_cb, SEEK_SET);
        fread(&block, sizeof(bk), 1, fp);
        if(!block.alloc)  size+=block.size;
        offset_cb = block.next;
    }
    fclose(fp);
    return size;
}

int no_of_free_blocks()
{
    FILE* fp = fopen("file_manager.dat","rb");
    int no = 0;
    bk block;
    fseek(fp, sizeof(mem_space), SEEK_SET);
    fread(&block, sizeof(bk), 1, fp);

    int offset_cb = block.next;
    if(!block.alloc)  no+=1;

    while (offset_cb != -1)
    {
        fseek(fp, offset_cb, SEEK_SET);
        fread(&block, sizeof(bk), 1, fp);
        if(!block.alloc)  no+=1;
        offset_cb = block.next;
    }
    fclose(fp);
    return no;
}
void delete_all_fb(file_header f_head)
{
    if(f_head.start_offset == -1)
    {
        return;
    }
    else
    {   
        FILE * fp = fopen("file_manager.dat", "rb+");
        file f;
        bk b;
        int offset_file = f_head.start_offset;
        while(offset_file != -1)
        {   
            int offset_bk = offset_file - sizeof(bk);
            fseek(fp, offset_file, SEEK_SET);
            fread(&f, sizeof(file), 1, fp);

            fseek(fp, offset_bk, SEEK_SET);
            fread(&b, sizeof(bk), 1, fp);
            b.alloc = 0;
            fseek(fp, offset_bk, SEEK_SET);
            fwrite(&b, sizeof(bk), 1, fp);

            free_bk(offset_bk);
            offset_file = f.next;
        }
        fclose(fp);
        update_m();
    }
}
void delete_file(const char* filename, const char* filetype)
{
    file_header f_head;
    FILE* fp = fopen("file_manager.dat","rb+");
    mem_space m;
    fseek(fp, 0, SEEK_SET);
    fread(&m, sizeof(mem_space), 1, fp);
    int found = 0, offset_f_h = m.file_offset;
    while(offset_f_h != -1)
    {   
        fseek(fp, offset_f_h, SEEK_SET);
        fread(&f_head, sizeof(file_header), 1, fp);
        if(!strcmp(f_head.file_name, filename) && !strcmp(f_head.file_type, filetype))
        {
            found = 1;
            break;
        }
        offset_f_h = f_head.next;
    }
    if(found)
    {
        delete_all_fb(f_head);
        fseek(fp, 0, SEEK_SET);
        fread(&m, sizeof(mem_space), 1, fp);
        bk fh_block;
        fseek(fp, offset_f_h - sizeof(bk), SEEK_SET);
        fread(&fh_block, sizeof(bk), 1, fp);

        fh_block.alloc = 0;
        fseek(fp, offset_f_h - sizeof(bk), SEEK_SET);
        fwrite(&fh_block, sizeof(bk), 1, fp);
        if((offset_f_h - sizeof(bk)) != sizeof(mem_space))
        free_bk(offset_f_h - sizeof(bk));
        fseek(fp, 0, SEEK_SET);
        fread(&m, sizeof(mem_space), 1, fp);

        if(f_head.prev != -1)
        {
            file_header prev_f_head;
            int offset_p_f_h = f_head.prev;
            fseek(fp, offset_p_f_h, SEEK_SET);
            fread(&prev_f_head, sizeof(file_header), 1, fp);
            prev_f_head.next = f_head.next;
            fseek(fp, offset_p_f_h, SEEK_SET);
            fwrite(&prev_f_head, sizeof(file_header), 1, fp);
        }
        if(f_head.next != -1)
        {   
            file_header next_f_head;
            int offset_n_f_h = f_head.next;
            fseek(fp, offset_n_f_h, SEEK_SET);
            fread(&next_f_head, sizeof(file_header), 1, fp);
            next_f_head.prev = f_head.prev;
            fseek(fp, offset_n_f_h, SEEK_SET);
            fwrite(&next_f_head, sizeof(file_header), 1, fp);
        }

        if(m.file_offset == offset_f_h)
        {
            m.file_offset = f_head.next;
        }
        m.size += fh_block.size;
        fseek(fp, 0, SEEK_SET);
        fwrite(&m, sizeof(mem_space), 1, fp);
        fclose(fp);
        verify_free_bk();
    }
    else
    {
        printf("\nNo such file present\n");
        fclose(fp);
        return;
    }
    
}

// void delete_fb(int offset_f)
// {
//     FILE* fp = fopen("file_manager.dat", "rb+");
//     bk block;
//     fseek(fp, offset_f - sizeof(bk), SEEK_SET);
//     fread(&block, sizeof(bk), 1, fp);
//     block.alloc = 0;

//     mem_space m;
//     fseek(fp, 0, SEEK_SET);
//     fread(&m, sizeof(mem_space), 1, fp);
//     m.size += block.size;
//     printf("\n%d\n", m.size);
//     fseek(fp, 0, SEEK_SET);
//     fwrite(&m, sizeof(mem_space), 1, fp);

//     fclose(fp);

// }
void free_bk(int offset_cb)
{
    FILE* fp = fopen("file_manager.dat", "rb+");
    bk c_block, p_block, n_block;
    fseek(fp, offset_cb, SEEK_SET);
    fread(&c_block, sizeof(bk), 1, fp);
    if(c_block.next != -1)
    {
        int offset_n = c_block.next;
        fseek(fp, offset_n, SEEK_SET);
        fread(&n_block, sizeof(bk), 1, fp);
        if(n_block.alloc == 0)
        {
            c_block.next = n_block.next;
            if(n_block.next != -1)
            {
                bk n_n_block;   
                fseek(fp, n_block.next, SEEK_SET);
                fread(&n_n_block, sizeof(bk), 1, fp);
                n_n_block.prev = n_block.prev;
                fseek(fp, n_block.next, SEEK_SET);
                fwrite(&n_n_block, sizeof(bk), 1, fp);
            }
            c_block.size += n_block.size + sizeof(bk);
        }
    }
    if(c_block.prev != -1)
    {   
        int offset_p = c_block.prev;
        fseek(fp, offset_p, SEEK_SET);
        fread(&p_block, sizeof(bk), 1, fp);
        if(p_block.alloc == 0)
        {
            p_block.next = c_block.next;
            if(c_block.next != -1)
            {
                fseek(fp, c_block.next, SEEK_SET);
                fread(&n_block, sizeof(bk), 1, fp);
                n_block.prev = c_block.prev;
                fseek(fp, c_block.next, SEEK_SET);
                fwrite(&n_block, sizeof(bk), 1, fp);
            }
            p_block.size += c_block.size + sizeof(bk);
        }
        fseek(fp, offset_p, SEEK_SET);
        fwrite(&p_block, sizeof(bk), 1, fp); 
    }
    fseek(fp, offset_cb, SEEK_SET);
    fwrite(&c_block, sizeof(bk), 1, fp);
    fclose(fp);
    update_m();
    return;
}
void free_space()
{
    FILE *fp = fopen("file_manager.dat", "r");
    mem_space m;
    fseek(fp, 0, SEEK_SET);
    fread(&m, sizeof(mem_space), 1, fp);
    printf("\nFREE SPACE: %d\n", m.size);
    fclose(fp);
}
void update_m()
{
    FILE *fp = fopen("file_manager.dat", "rb+");
    mem_space m;
    fseek(fp, 0, SEEK_SET);
    fread(&m, sizeof(mem_space), 1, fp);
    m.size = size_of_free_blocks();
    fseek(fp, 0, SEEK_SET);
    fwrite(&m, sizeof(mem_space), 1, fp);
    fclose(fp);
}
void print_file_structure()
{
    FILE* fp = fopen("file_manager.dat", "rb");
    mem_space m;
    fseek(fp, 0, SEEK_SET);
    fread(&m, sizeof(mem_space), 1, fp);
    if(m.file_offset == -1)
    {
        printf("No files present.\n");
        fclose(fp);
        return;
    }
    file_header f_head;
    fseek(fp, m.file_offset, SEEK_SET);
    fread(&f_head, sizeof(file_header), 1, fp);
    while(f_head.next != -1)
    {
        printf("\n %s.%s\n", f_head.file_name, f_head.file_type);
        if(f_head.start_offset != -1)
        {
            printf("\nFILE CONTENTS\n");
            int offset_file = f_head.start_offset;
            while (offset_file != -1)
            {
                file f;
                fseek(fp, offset_file, SEEK_SET);
                fread(&f, sizeof(file), 1, fp);
                int t_size = f.end_of_block - offset_file - sizeof(file);
                char* temp = malloc(sizeof(char)*(t_size+1));
                temp[t_size] = '\0'; // JUST IN CASE
                fseek(fp, offset_file + sizeof(file), SEEK_SET);
                fread(temp, sizeof(char), t_size, fp);
                printf("\n\n%s\n\n", temp);
                offset_file = f.next;
                free(temp);
            }
            
        }
        fseek(fp, f_head.next, SEEK_SET);
        fread(&f_head, sizeof(file_header), 1, fp);
    }
    printf("\n %s.%s\n", f_head.file_name, f_head.file_type);
    if(f_head.start_offset != -1)
    {
        printf("\nFILE CONTENTS\n");
        int offset_file = f_head.start_offset;
        while (offset_file != -1)
        {
            file f;
            fseek(fp, offset_file, SEEK_SET);
            fread(&f, sizeof(file), 1, fp);
            int t_size = f.end_of_block - offset_file - sizeof(file);
            char* temp = malloc(sizeof(char)*(t_size+1));
            temp[t_size] = '\0'; // JUST IN CASE
            fseek(fp, offset_file + sizeof(file), SEEK_SET);
            fread(temp, sizeof(char), t_size, fp);
            printf("\n\n%s\n\n", temp);
            offset_file = f.next;
            free(temp);
        }
        
    }
    fclose(fp);
    return;

}