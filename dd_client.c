#include "headers.h"

int main()
{
    int c;
    //printf("%lu\n\n",sizeof(int));
    init_manager();
    while(1)
    {
        printf("\n1. Create File\n2. Print the current file structure\n3. Insert into file\n4. Read file\n5. Delete File\n6. Exit\n\n");
        scanf("%d", &c);
        switch (c)
        {
        case 1:
            printf("\nEnter File Name: ");
            char *file_name = malloc(sizeof(char)*25);
            scanf("%s", file_name);
            
            create_file(file_name);
            free(file_name);
            break;
        case 2:
            print_file_structure();
            break;

        case 3:
            printf("\nEnter File Name: ");
            char *file_name2 = (char*)malloc(sizeof(char)*25);
            scanf("%s",file_name2);
            char mode;
            printf("\nEnter mode: ");
            fflush(stdin);
            scanf("%c",&mode);
            printf("\nEnter file contents: \n");
            char *contents = (char*)malloc(sizeof(char)*1000);
            fflush(stdin);
            scanf("%[^\n]%*c",contents);
            //printf("%lu\n",strlen(contents));
            insert_into_file(file_name2,contents,mode);
            fflush(stdin);
            free(file_name2);
            break;

        case 4:
            printf("\nEnter File Name: ");
            char *file_name3 = (char*)malloc(sizeof(char)*25);
            scanf("%s",file_name3);
            read_file(file_name3);
            free(file_name3);
            break;

        case 5:
            printf("\nEnter File Name: ");
            char *file_name4 = (char*)malloc(sizeof(char)*25);
            scanf("%s",file_name4);
            int res = del_file(file_name4,0);
            if(res==-1)
            {
                printf("file doesn't exist\n");
            }
            free(file_name4);
            break;
        
        default:
            return 0;
        }
    }
    
}