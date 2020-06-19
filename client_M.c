#include "headers_M.h"

int main()
{
    int c;
    printf("%d\n\n",sizeof(int));
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
            char *file_ext = malloc(sizeof(char)*8);
            scanf("%s", file_name);
            printf("\nEnter File Ext: ");
            scanf("%s", file_ext);
            create_file(file_name, file_ext);
            break;
        case 2:
            print_file_structure();
            break;
        
        default:
            break;
        }
    }
    
}