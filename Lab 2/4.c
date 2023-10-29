#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum status_code 
{
    success,
    invalid_parameter,
    file_error,
    allocate_error,
} status_code;

status_code find_substr_in_file(const char* file_path, const char* substr, bool* is_found) 
{
    FILE* file = fopen(file_path, "r");
    if (file == NULL) return file_error;
    if (feof(file)) 
    {
        *is_found = false;
        return success;
    }
    
    char* line = NULL;
    *is_found = false;
    int getline_res;

    while ((getline_res = getline(&line, &(size_t){0}, file)) != -1) 
    {
        char *found = strstr(line, substr);
        if (found != NULL) 
        {
            *is_found = true;
            free(line);
            fclose(file);
            return success;
        }
    }

    free(line);
    fclose(file);
    return success;
}

status_code get_names(const char* names_file_path, char*** names, int* names_size) 
{
    *names_size = 0;
    FILE* names_file = fopen(names_file_path, "r");
    if (names_file == NULL) return file_error;
    if (feof(names_file)) return success;

    int names_capacity = 32;
    int row_capacity = 32, names_index = 0, row_index = 0;

    *names = (char**)malloc(sizeof(char*) * names_capacity);
    if (*names == NULL) 
    {
        fclose(names_file);
        return allocate_error;
    }

    char* tmp = (char*)malloc(sizeof(char) * row_capacity);
    if (tmp == NULL) 
    {
        fclose(names_file);
        free(*names);
        return allocate_error;
    }

    bool flag = false;
    char c = fgetc(names_file);
    while (true) 
    {
        if (isspace(c) || c == '\n' || c == EOF) 
        {
            if (flag) 
            {
                flag = false;
                tmp[row_index] = '\0';
                (*names)[names_index] = strdup(tmp);
                ++names_index;

                if (names_index == names_capacity - 1) 
                {
                    names_capacity *= 2;
                    char** temp = (char**)realloc(*names, sizeof(char*) * names_capacity);
                    if (temp == NULL) 
                    {
                        fclose(names_file);
                        free(tmp);
                        free(*names);
                        return allocate_error;
                    }
                    *names = temp;
                }

                free(tmp);
                tmp = (char*)malloc(sizeof(char) * row_capacity);
                if (tmp == NULL) 
                {
                    fclose(names_file);
                    free(*names);
                    return allocate_error;
                }

                row_index = 0;
                if (c == EOF) break;
            }
            c = fgetc(names_file);
        } 
        else if (!isspace(c)) 
        {
            flag = true;
            tmp[row_index] = c;
            ++row_index;
            if (row_index == row_capacity - 1) 
            {
                row_index *= 2;
                char* temp = (char*)realloc(tmp, row_capacity);
                if (temp == NULL) 
                {
                    fclose(names_file);
                    free(tmp);
                    free(*names);
                    return allocate_error;
                }
                tmp = temp;
            }
            c = fgetc(names_file);
        }
    }
    fclose(names_file);
    free(tmp);
    *names_size = names_index;
    return success;
}

int main(int argc, char *argv[]) 
{
    if (argc < 3) 
    {
        printf("invalid number of arguments\n");
        return 1;
    }

    const char* names_file_path = argv[1];
    const char* substr = argv[2];

    char** names = NULL;
    int names_size;
    if (get_names(names_file_path, &names, &names_size)) 
    {
        printf("allocate error\n");
        return 1;
    }

    pid_t pid;
    int cnt = 0;
    for (int i = 0; i < names_size; ++i) 
    {
        const char* current_file_path = names[i];

        pid = fork();

        if (pid == 0) 
        {
            bool is_found;
            status_code status = find_substr_in_file(current_file_path, substr, &is_found);
            if (status == file_error) 
            {
                printf("file error\n");
                free(names);
                return 1;
            }
            
            if (is_found) 
            {
                ++cnt;
                printf("%s\n", current_file_path);
            }
            _Exit(cnt);
        } 
        else if (pid == -1) 
        {
            return -1;
        } 
        else 
        {
            if (!current_file_path) 
            {
                printf("allocate error\n");
                return 1;
            }
        }
    }

    pid_t child_pid;
    int status;
    while ((child_pid = wait(&status)) > 0)
    {
        if (WIFEXITED(status)) cnt += WEXITSTATUS(status);
    }

    if (!cnt) 
    {
        printf("line is not found in any file\n");
    }

    free(names);
    return 0;
}