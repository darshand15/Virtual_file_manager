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
        b.size = m.size - sizeof(bk);
        fseek(fp, sizeof(mem_space), SEEK_SET);
        fwrite(&b, sizeof(bk), 1, fp);
        fclose(fp);
    }
    
}

bk worst_block(FILE* fp, int r_size) 
{  
    // Initialise a pointer to the head mem_space
    mem_space head_mem_space;
    fseek(fp, 0, SEEK_SET);
    fread(&head_mem_space, sizeof(mem_space), 1, fp);
    /* Get largest unallocated block in list*/
 
    bk index, max;
    int max_flag = 0;
    printf("\n\n%d\n\n", sizeof(mem_space));
    fseek(fp, sizeof(mem_space), SEEK_SET);
    fread(&index, sizeof(bk), 1, fp);
    // max pointer would point to the block with largest unallocated space
    // printf("\nIN WB\n %d %d %d %d\n", index.alloc, index.next, index.prev, index.size);

    if(index.next == -1)
    {
        return index;
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
        }
        
        do
        {
            fseek(fp, index.next, SEEK_SET);
            fread(&index, sizeof(bk), 1, fp);
            if(!(index.alloc) && (index.size > max.size) ) // if unallocated and size is greater than max size
            {
                max = index;
                max_flag = 1;
            }
            // index = (bk*)(temp + index.next);
            // printf("\nIN WB\n %d %d %d %d\n", index.alloc, index.next, index.prev, index.size);

            // printf("\nIN WB\n %d %d %d %d\n", index.alloc, index.next, index.prev, index.size);

        } while(index.next != -1);
    }
    // printf("\nIN WB\n %d %d %d %d\n", max.alloc, max.next, max.prev, max.size);

    if(max_flag && max.size >= r_size)     // return max block only if the size is greater than the required size
    {
        return max;
    } 
    else
    {
        //TO DO
    }
}

void create_file(const char* filename, const char* filetype)
{
    FILE* fp = fopen("file_manager.dat", "rb+");
    mem_space m;
    fseek(fp, 0, SEEK_SET);
    fread(&m, sizeof(mem_space), 1, fp);
    int r_size = sizeof(file_header);
    bk block = worst_block(fp, r_size);
    int offset_cb;
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
    
    if(block.size > (r_size + sizeof(bk)))
    {
        int offset_fb = offset_cb + sizeof(bk) + r_size;
        printf("\n\nfb: %d cb: %d\n\n", offset_fb, offset_cb);
        bk free_block;
        free_block.next = block.next;
        block.next = offset_fb;
        free_block.prev = offset_cb;
        free_block.size = block.size - sizeof(bk) - r_size;
        free_block.alloc = 0;
        block.size = r_size;
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
        printf("\n%d\n", p_file.next);
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
    m.size -= (r_size + sizeof(bk));
    fseek(fp, 0, SEEK_SET);
    fwrite(&m, sizeof(mem_space), 1, fp);

    fseek(fp, offset_cb, SEEK_SET);
    fwrite(&block, sizeof(bk), 1, fp);
    fclose(fp);
}

// void create_f_b(file_header f_head, int r_size)
// {

// }

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
    if(m.size > (r_size + sizeof(bk)))  // Assumes only one file block required
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
                offset_file = f_head.start_offset;
                file f;
                f.next = -1;
                f.prev = -1;
                bk block = worst_block(fp, r_size);
                int offset_cb;
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
                    printf("\n\nfb: %d cb: %d\n\n", offset_fb, offset_cb);
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
                break;
            case 'a':
                printf("\n IN APPEND OFFSET:%d\n", offset_file);
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
                    printf("\n\nOFFSET OF PF: %d\n\n", offset_p_f);
                    file p_f;
                    fseek(fp, offset_p_f, SEEK_SET);
                    fread(&p_f, sizeof(file), 1, fp);
                    bk block = worst_block(fp, r_size);
                    file f;
                    f.next = -1;
                    f.prev = offset_p_f;
                    int offset_cb;
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
                        printf("\n\nin append fb: %d cb: %d\n\n", offset_fb, offset_cb);
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
            printf("\nIN DELETE\n");
            int offset_bk = offset_file - sizeof(bk);
            fseek(fp, offset_file, SEEK_SET);
            fread(&f, sizeof(file), 1, fp);

            fseek(fp, offset_bk, SEEK_SET);
            fread(&b, sizeof(bk), 1, fp);
            b.alloc = 0;
            fseek(fp, offset_bk, SEEK_SET);
            fwrite(&b, sizeof(bk), 1, fp);

            mem_space m;
            fseek(fp, 0, SEEK_SET);
            fread(&m, sizeof(mem_space), 1, fp);
            m.size += b.size + sizeof(bk);
            printf("\n%d\n", m.size);
            fseek(fp, 0, SEEK_SET);
            fwrite(&m, sizeof(mem_space), 1, fp);

            offset_file = f.next;
        }
        fclose(fp);
    }
    
}

void free_space()
{
    FILE * fp = fopen("file_manager.dat", "r");
    mem_space m;
    fseek(fp, 0, SEEK_SET);
    fread(&m, sizeof(mem_space), 1, fp);
    printf("\n FREE SPACE: %d\n", m.size);
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