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
    // if(*temp == NULL)
    // {
    //     printf("No More Space Available\n");
    //     return;
    // }


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
        // mem_space* m = (mem_space*)*temp;
        mem_space m;
        m.size = 1000 - sizeof(mem_space);
        m.free_head = sizeof(mem_space);
        m.file_offset = -1;
        m.current_offset = 0;
        fseek(fp, 0, SEEK_SET);
        fwrite(&m, sizeof(mem_space), 1, fp);
        
        bk b;
        // bk* b = (bk*)(*temp + sizeof(mem_space));
        b.next = -1;
        b.prev = -1;
        b.alloc = 0;
        b.size = m.size - sizeof(bk);
        fseek(fp, m.free_head, SEEK_SET);
        fwrite(&b, sizeof(bk), 1, fp);
        fclose(fp);
    }
    
}

bk worst_block(FILE* fp, int r_size) 
{
    
    // mem_space* head_mem_space = (mem_space*)(temp);   
    // Initialise a pointer to the head mem_space
    mem_space head_mem_space;
    fseek(fp, 0, SEEK_SET);
    fread(&head_mem_space, sizeof(mem_space), 1, fp);
    /* Get largest unallocated block in list*/

    // bk* index = (bk*)(temp + head_mem_space->free_head), *max = NULL;   
    bk index, max;
    int max_flag = 0;
    fseek(fp, head_mem_space.free_head, SEEK_SET);
    fread(&index, sizeof(bk), 1, fp);
    // max pointer would point to the block with largest unallocated space
    printf("\nIN WB\n %d %d %d %d\n", index.alloc, index.next, index.prev, index.size);

    if(index.next == -1)
    {
        return index;
    }
    else
    {
        while(index.next != -1)
        {
            if(!(index.alloc) && (!max_flag || index.size > max.size) ) // if unallocated and size is greater than max size
            {
                max = index;
                max_flag = 1;
            }
            // index = (bk*)(temp + index.next);
            fseek(fp, index.next, SEEK_SET);
            fread(&index, sizeof(bk), 1, fp);
        }
    }

    if(!max_flag || max.size >= r_size)     // return max block only if the size is greater than the required size
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
    // printf("\n\n%d\n\n", block.prev);
    // int offset_cb = (block.prev == -1)?sizeof(mem_space):((bk*)temp + block.prev)->next;
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
        // printf("\n\n%d\n\n", offset_cb);
        // bk* free_block = (bk*)(temp + offset_fb);
        bk free_block;
        free_block.next = block.next;
        block.next = offset_fb;
        free_block.prev = offset_cb;
        m.free_head = offset_fb;
        free_block.size = block.size - sizeof(bk) - r_size;
        free_block.alloc = 0;
        block.size = r_size;
        fseek(fp, offset_fb, SEEK_SET);
        fwrite(&free_block, sizeof(bk), 1, fp);
        fseek(fp, m.current_offset, SEEK_SET);
        fwrite(&m, sizeof(mem_space), 1, fp);
    }
    if(m.file_offset == -1)
    {
        m.file_offset = offset_cb + sizeof(bk);
        // printf("\n\n%d\n\n", m.file_offset);
        // file_header* file_h = (file_header*)(temp + m.file_offset);
        file_header file_h;
        file_h.next = -1;
        file_h.file_id = 0;
        // file_h.file_name = malloc(sizeof(char)*(strlen(filename)+1));
        strcpy(file_h.file_name, filename);
        // file_h.file_type = malloc(sizeof(char)*(strlen(filetype)+1));
        strcpy(file_h.file_type, filetype);
        file_h.start_offset = -1;
        file_h.end_offset = -1;
        file_h.prev = -1;
        block.alloc = 1;

        fseek(fp, offset_cb, SEEK_SET);
        fwrite(&block, sizeof(bk), 1, fp);

        fseek(fp, m.file_offset, SEEK_SET);
        fwrite(&file_h, sizeof(file_header), 1, fp);

        fseek(fp, m.current_offset, SEEK_SET);
        fwrite(&m, sizeof(mem_space), 1, fp);
    }
    else
    {
        // file_header* file_h = (file_header*)(temp + offset_cb + sizeof(bk));
        file_header file_h;
        int c_file_offset = offset_cb + sizeof(bk);
        file_h.file_id = 0;
        // file_h.file_name = malloc(sizeof(char)*(strlen(filename)+1));
        strcpy(file_h.file_name, filename);
        // file_h.file_type = malloc(sizeof(char)*(strlen(filetype)+1));
        strcpy(file_h.file_type, filetype);
        file_h.start_offset = -1;
        file_h.end_offset = -1;
        // file_header* p_file = (file_header*)(temp + m.file_offset);
        file_header p_file;
        fseek(fp, m.file_offset, SEEK_SET);
        fread(&p_file, sizeof(file_header), 1, fp);

        int p_file_offset = m.file_offset;
        printf("\n%d\n", p_file.next);
        while(p_file.next != -1)
        {
            p_file_offset = p_file.next;
            // p_file = (file_header*)(temp + p_file_offset);
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
    fseek(fp, offset_cb, SEEK_SET);
    fwrite(&block, sizeof(bk), 1, fp);
    fclose(fp);
}
// void insert_into_file(const char* filename, const char* filetype, char* temp)
// {
//     mem_space* m = (mem_space*)temp;
//     if(m.file_offset == -1)
//     {
//         printf("\nNo file has been created.\n");
//         return;
//     }
//     file_header* temp_file_pointer = (file_header*)(temp + m.file_offset);
//     while(temp_file_pointer->next != -1)
//     {
//         if(strcmp(filename, temp_file_pointer->file_name) && strcmp(filetype, temp_file_pointer->file_type))
//         {
//             break;
//         }
//         temp_file_pointer = (file_header*)(temp + temp_file_pointer->end_offset); 
//     }

// }
void print_file_structure()
{
    FILE* fp = fopen("file_manager.dat", "rb");
    mem_space m;
    fseek(fp, 0, SEEK_SET);
    fread(&m, sizeof(mem_space), 1, fp);
    if(m.file_offset == -1)
    {
        printf("No files present.\n");
        return;
    }
    // file_header* f_head = (file_header*)(temp + m.file_offset);
    file_header f_head;
    fseek(fp, m.file_offset, SEEK_SET);
    fread(&f_head, sizeof(file_header), 1, fp);
    while(f_head.next != -1)
    {
        printf("\n %s.%s\n", f_head.file_name, f_head.file_type);
        // f_head = (file_header*)(temp + f_head.next);
        fseek(fp, f_head.next, SEEK_SET);
        fread(&f_head, sizeof(file_header), 1, fp);
    }
    printf("\n %s.%s\n", f_head.file_name, f_head.file_type);
    fclose(fp);
    return;

}