#ifndef header_h
#define header_h

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct mem_space
{
    int size; // size of the page/block
    int alloc; // 1 if the block is allocated
    int file_offset; //offset to the first file header
    int free_head; //offset to the first available empty chunk of bytes
    int next; //offset to the next mem_space structure
   // int prev; //offset to the previous mem_space structure
    int current_offset; // offset of the current structure

};

typedef struct mem_space mem_space;

struct file_header
{
    int file_id; //unique id to identify the file
    char* file_type;
    char* file_name;
    int start_offset;
    int end_offset; //signifies the EOF
    int prev;
    int next;
};
typedef struct file_header file_header;

struct file
{
    int prev;
    int end_of_block;
    int next;
};

typedef struct file file;

#endif