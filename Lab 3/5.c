#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_PATH_SIZE 256

typedef enum status_code 
{
    SUCCESS,
    ARGUMENTS_ERROR,
    DIRECTORY_ERROR,
    FILE_ERROR,
} status_code;

void print_error(const status_code st) 
{
    if (st == ARGUMENTS_ERROR) 
    {
        printf("Invalid number of arguments\n");
    }
    if (st == DIRECTORY_ERROR) 
    {
        printf("Error while opening a directory\n");
    }
    if (st == FILE_ERROR) 
    {
        printf("Error while retrieving file information\n");
    }
}

status_code process_file(const char* fullpath, const char* filename) 
{
    struct stat statbuf;
    if (stat(fullpath, &statbuf) == -1) 
    {
        return FILE_ERROR;
    }

    printf("   %s ", filename);

    if (S_ISDIR(statbuf.st_mode)) 
    {
        printf("(directory)\n");
    } 
    else if (S_ISREG(statbuf.st_mode)) 
    {
        printf("(file)\n");
    } 
    else if (S_ISLNK(statbuf.st_mode)) 
    {
        printf("(symlink)\n");
    } 
    else if (S_ISFIFO(statbuf.st_mode)) 
    {
        printf("(FIFO)\n");
    } 
    else if (S_ISSOCK(statbuf.st_mode)) 
    {
        printf("(socket)\n");
    } 
    else if (S_ISCHR(statbuf.st_mode)) 
    {
        printf("(character device)\n");
    } 
    else if (S_ISBLK(statbuf.st_mode)) 
    {
        printf("(block device)\n");
    }
    else
    {
        printf("(unknown)\n");
    }

    return SUCCESS;
}

status_code list_dir(const char* path) 
{
    struct dirent* entry;
    DIR* dir = opendir(path);

    if (dir == NULL) 
    {
        return DIRECTORY_ERROR;
    }

    printf("%s\n", path);

    while ((entry = readdir(dir)) != NULL) 
    {
        char fullpath[MAX_PATH_SIZE];
        struct stat statbuf;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (process_file(fullpath, entry->d_name) != SUCCESS) 
        {
            closedir(dir);
            return FILE_ERROR;
        }
    }

    closedir(dir);

    return SUCCESS;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2) 
    {
        print_error(ARGUMENTS_ERROR);
        return ARGUMENTS_ERROR;
    }

    for (int i = 1; i < argc; ++i) 
    {
        const status_code st = list_dir(argv[i]);
        if (st != SUCCESS) 
        {
            print_error(st);
            return st;
        }
    }

    return SUCCESS;
}