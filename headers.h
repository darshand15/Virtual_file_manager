#ifndef header_h
#define header_h

#include<stdio.h>
#include<stdlib.h>

struct mem_space
{
    int size; // size of the page which is allocated - this is our memory space
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
    int prev;
    int end_of_block;
    int next;
};

typedef struct file file;

#endif