#include "headers.h"

int main()
{
    int c;
    //printf("%lu\n\n",sizeof(int));
    init_manager();
    while(1)
    {
        printf("1. Create File\n2. Print the current file structure\n3. Exit\n");
        scanf("%d", &c);
        switch (c)
        {
        case 1:
            printf("\nEnter File Name: ");
            char *file_name = malloc(sizeof(char)*25);
            scanf("%s", file_name);
            
            create_file(file_name);
            break;
        case 2:
            print_file_structure();
            break;
        
        default:
            return 0;
        }
    }
    
}