#include "headers_M.h"

int main()
{
    int c;
    printf("%d\n\n",sizeof(int));
    init_manager();
    while(1)
    {
        printf("1. Create File\n2. Print the current file structure\n3. Edit\n4. Free Space Left\n5. Delete File\n6. Check Block Integrity\n");
        scanf("%d", &c);
        char *file_name = malloc(sizeof(char)*25);
        char *file_ext = malloc(sizeof(char)*8);
        switch (c)
        {
        case 1:
            printf("\nEnter File Name: ");
            scanf("%s", file_name);
            printf("\nEnter File Ext: ");
            scanf("%s", file_ext);
            create_file(file_name, file_ext);

            break;
        case 2:
            print_file_structure();
            break;
        case 3:
            printf("\nEnter File Name: ");
            scanf("%s", file_name);
            printf("\nEnter File Ext: ");
            scanf("%s", file_ext);
            char mode;
            printf("\nMode of edit: ");
            fflush(stdin);
            scanf("%c", &mode);
            if(mode=='r')
            {   
                char text[1] = "\0";
                edit_file(file_name, file_ext, text, mode);
            }
            else
            {
                char* text = malloc(sizeof(char)*1000);
                printf("\nEnter text: \n");
                scanf("%s", text);
                edit_file(file_name, file_ext, text, mode);
            }
            
            break;
        case 4:
            free_space();
            break;
        case 5:
            printf("\nEnter File Name: ");
            scanf("%s", file_name);
            printf("\nEnter File Ext: ");
            scanf("%s", file_ext);
            delete_file(file_name, file_ext);
            break;
        case 6:
            check_block_integrity();
            break;
        default:
            break;
        }            
        free(file_name);
        free(file_ext);
    }
    
}