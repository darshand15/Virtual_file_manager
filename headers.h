#ifndef header_h
#define header_h

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>

struct mem_space
{
    int free_size; //available bytes in the page
    int files; //offset to the first file header
    int free_head; //offset to the first available empty chunk of bytes

};

typedef struct mem_space mem_space;

struct file_header
{
    char *file_id; //unique name to identify the file
    char *file_type; 
    int start_offset; 
    int end_offset; //signifies the EOF
    int prev; //offset to the previous file header
    int next; //offset to the next file header
};
typedef struct file_header file_header;

struct file
{
    int prev; // offset to a previous block which can either be a different file or part of the same file
    int end_of_block;// signifies the end of the current block and not necessarily the EOF
    int next; //offset to the next block which can either be a different file or part of the same file
};

typedef struct file file;

struct book_keeper
{
    int prev;//offset to the previous book keeping structure
    int next;//offset to the next book keeping structure
    int size;//size of the block signified by the book keeping structure
    char alloc_f;//holds 'a' if the block is already allocated and 'f' if its free
};

typedef struct book_keeper book_keeper;

#endif