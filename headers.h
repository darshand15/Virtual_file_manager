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
    //int free_head; //offset to the first available empty chunk of bytes

};

typedef struct mem_space mem_space;

struct file_header
{
    char file_id[32]; //unique name to identify the file
    char file_type[8]; 
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

int file_exists(const char* file_name);

void init_space(char **temp, int n);

void init_manager();

int best_fit(FILE *fp,int req_size);

void create_file(const char* file_name);

void insert_into_file(const char* file_name,char *content,char mode);
//mode is 'a' if append, 'w' if write mode

void print_file_structure();

int largest_available_block(FILE *fp);

int del_file(const char* file_name,int del_mode);
//del_mode has the value 0 if everything including the file contents and the file header has to be deleted
//del_mode has the value 1 if only the file contents has to be deleted and the file header isn't


#endif