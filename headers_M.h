#ifndef header_h
#define header_h

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <windows.h>



struct mem_space
{
    int size; // size of the page/block
    int file_offset; //offset to the first file header
};

typedef struct mem_space mem_space;

typedef struct book_keeper
{
    int next; //offset to the next book keeping structure
    int prev; // offset to the previous book keeping structure
    int alloc; // 1 if the block is allocated
    int size;
    // int current_offset;
} bk;


struct file_header
{
    int start_offset;
    int end_offset; //signifies the EOF
    int prev;
    int next;
    int file_id; //unique id to identify the file
    char file_type[8];
    char file_name[32];
};
typedef struct file_header file_header;

struct file
{
    int prev;
    int end_of_block;
    int next;
};

typedef struct file file;


int exists(const char *fname);
void init_space(char** temp, int n);
void init_manager();
int worst_block(FILE* fp, int r_size);
int create_file(const char* filename, const char* filetype);
void print_file_structure();
void create_f_b(file_header f_head, int r_size);
void delete_all_fb(file_header f_head);
void edit_file(const char* filename, const char* filetype, char* s, char mode);
void edit_file_2(const char* filename, const char* filetype, char* s, char mode, int size);
void delete_file(const char* filename, const char* filetype);
void delete_fb(int offset_f);
int no_of_free_blocks();
int size_of_free_blocks();
void check_block_integrity();
void free_space();
void free_bk(int offset_cb);
void update_m();
void verify_free_bk();
int check_file_exists(const char* filename, const char* filetype);
int open_using_ext_app(const char* filename, const char* filetype, const char* app);
int open_existing_file(char* file_path, char* filename, char* filetype);
#endif