#include "headers.h"

int main()
{
    int c,flag;
    //printf("%lu\n\n",sizeof(int));
    //printf("%lu %lu %lu %lu",sizeof(mem_space),sizeof(file_header),sizeof(file),sizeof(book_keeper));
    char *alloc = init_manager();
    while(1)
    {
        printf("\n1. Create File\n2. Print the current file structure\n3. Insert into file\n4. Read file\n5. Delete File\n6. print bk structure\n7. Exit\n\n");
        scanf("%d", &c);
        switch (c)
        {
        case 1:
            flag = 1;
            printf("\nEnter File Name: ");
            char *file_name;
            while(flag==1)
            {
                file_name = malloc(sizeof(char)*25);
                scanf("%s", file_name);
                flag = check_file_exists(file_name);
                if(flag)
                {
                    printf("\nFile already exists\nDo you want to replace the file?(yes or no)\n");
                    char *choice = malloc(sizeof(char)*4);
                    scanf("%s",choice);
                    
                    if(strcmp(choice,"yes")==0)
                    {
                        int res_d = del_file(file_name,0);
                        flag = 0;
                    }
                    else if(strcmp(choice,"no")==0)
                    {
                        printf("\nEnter a new file name: ");
                        free(file_name);
                    }
                    free(choice);
                }

            }
            
            int res_c = create_file(file_name);
            free(file_name);
            break;

        case 2:
            print_file_structure();
            break;

        case 3:
            printf("\nEnter File Name: ");
            char *file_name2 = (char*)malloc(sizeof(char)*25);
            scanf("%s",file_name2);
            int f_e = check_file_exists(file_name2);
            char mode;
            printf("\nEnter mode: ");
            fflush(stdin);
            scanf("%c",&mode);

            if(f_e==1)
            {
                printf("\nEnter file contents: \n");
                char *contents = (char*)malloc(sizeof(char)*11);
                char ch;
                int i = 0;
                
                while((ch = getchar())!='\0')
                {
                    //printf("test");
                    if(i%10==0)
                    {
                        contents = realloc(contents,i + 10 + 1);
                    }
                    contents[i] = ch;
                    i++;
                }

                contents[i] = '\0';
                //printf("%s\n",contents);
                fflush(stdin);
                
                insert_into_file(file_name2,contents,mode);
                fflush(stdin);
                free(file_name2);
                free(contents);
            }
            else if(f_e==0 && mode=='w')
            {
                int r_c_f = create_file(file_name2);
                if(r_c_f!=-1)
                {
                    printf("\nEnter file contents: \n");
                    char *contents = (char*)malloc(sizeof(char)*11);
                    char ch;
                    int i = 0;
                    
                    while((ch = getchar())!='\0')
                    {
                        //printf("test");
                        if(i%10==0)
                        {
                            contents = realloc(contents,i + 10 + 1);
                        }
                        contents[i] = ch;
                        i++;
                    }

                    contents[i] = '\0';
                    //printf("%s\n",contents);
                    fflush(stdin);
                    
                    insert_into_file(file_name2,contents,mode);
                    fflush(stdin);
                    free(file_name2);
                    free(contents);
                }
            }
            else if(f_e==0 && mode=='a')
            {
                printf("\nFile does not exist\n");
                free(file_name2);
            }
            
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

        case 6:
            print_bk();
            break;
        
        default:
            if(alloc!=NULL)
            {
                free(alloc);
            }
            return 0;
        }
    }
    
}

//char *a = "abcdefghijklmnopqrstuvwxyz";
//abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789abc