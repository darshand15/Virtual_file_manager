#ifndef header_h
#define header_h

#include<stdio.h>
#include<stdlib.h>

struct mem_space
{
    int free_size; //available bytes in the page
    int files; //offset to the first file header
    int free_head; //offset to the first available empty chunk of bytes

};

typedef struct mem_space mem_space;

struct file_header
{
    int file_id; //unique id to identify the file
    char *file_type; 
    int start_offset; 
    int end_offset; //signifies the EOF
};
typedef struct file_header file_header;

struct file
{
    int prev; // offset to a previous block which can either be a different file or part of the same file
    int end_of_block;// signifies the end of the current block and not necessarily the EOF
    int next; //offset to the next block which can either be a different file or part of the same file
};

typedef struct file file;

#endif