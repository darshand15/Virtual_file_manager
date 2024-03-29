#include "headers_M.h"

int main()
{
    int c;
    init_manager();
    while(1)
    {
        printf("\n1. Create File\n2. Print the current file structure\n3. Edit\n4. Free Space Left\n5. Delete File\n6. Check Block Integrity\n7. Open Using External Application\n8. Open Existing File\n");
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
            if(check_file_exists(file_name, file_ext) == 0)
            {
                create_file(file_name, file_ext);
            }
            else
            {
                printf("\nThere is already a file with the same name and type, Enter 1 if you want to overwrite else 0?\n");
                int c;
                scanf("%d", &c);
                if(c)
                {
                    delete_file(file_name, file_ext);
                    create_file(file_name, file_ext);
                }
            }
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
                int f_e = check_file_exists(file_name, file_ext);
                if(f_e==1)
                {
                    printf("\nEnter file contents: \n");
                    char *contents = (char*)malloc(sizeof(char)*11);
                    char ch;
                    int i = 0;
                    
                    while((ch = getchar())!='|')
                    {
                        if(i%10==0)
                        {
                            contents = realloc(contents,i + 10 + 1);
                        }
                        contents[i] = ch;
                        i++;
                    }

                    contents[i] = '\0';
                    fflush(stdin);
                    
                    edit_file(file_name, file_ext, contents, mode);
                    fflush(stdin);
                    free(contents);
                }
                else if(f_e==0 && mode=='w')
                {
                    int r_c_f = create_file(file_name, file_ext);
                    if(r_c_f!=-1)
                    {
                        printf("\nEnter file contents: \n");
                        char *contents = (char*)malloc(sizeof(char)*11);
                        char ch;
                        int i = 0;
                        
                        while((ch = getchar())!='|')
                        {
                            if(i%10==0)
                            {
                                contents = realloc(contents,i + 10 + 1);
                            }
                            contents[i] = ch;
                            i++;
                        }

                        contents[i] = '\0';
                        fflush(stdin);
                        edit_file(file_name, file_ext, contents, mode);
                        fflush(stdin);
                        free(contents);
                    }
                }
                else if(f_e==0 && mode=='a')
                {
                    printf("\nFile does not exist\n");
                }
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
        case 7:
            printf("\nEnter File Name: ");
            scanf("%s", file_name);
            printf("\nEnter File Ext: ");
            scanf("%s", file_ext);
            char* app = malloc(sizeof(char)*256);
            printf("\nEnter 1 if you want to use the default application else 0\n");
            int c;
            scanf("%d", &c);
            if(c)
            {
                app[0] = '\0';
            }
            else
            {
                printf("\nEnter Application path: ");
                scanf("%s", app);
            }
            open_using_ext_app(file_name, file_ext, app);
            free(app);
            break;
        case 8:
            printf("\nEnter File Path: ");
            char* file_path = malloc(sizeof(char)*256);
            scanf("%s", file_path);
            printf("\nEnter 1 if you want to use the same file name else 0\n");
            int d;
            scanf("%d", &d);
            if(d)
            {
                file_name[0] = '\0';
                file_ext[0] = '\0';
                open_existing_file(file_path, file_name, file_ext);
            }
            else
            {
                printf("\nEnter File Name: ");
                scanf("%s", file_name);
                printf("\nEnter File Ext: ");
                scanf("%s", file_ext);
                open_existing_file(file_path, file_name, file_ext);
            }
            free(file_path);
            break;
        default:
            break;
        }            
        free(file_name);
        free(file_ext);
    }
    
}