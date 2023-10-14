#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>

typedef enum status_code {
    success,
    file_error,
} status_code;

typedef enum commands 
{
    XOR8,
    XOR32,
    MASK,
    UNKNOWN,
    INVALID_NUMBER_OF_ARGS
} commands;

status_code xor8(const char* path_file, int* res) 
{
    FILE *file = fopen(path_file, "rb");
    if (file == NULL) return file_error;
    char byte;
    *res = 0;
    while (fread(&byte, sizeof(char), 1, file)) *res ^= (int)byte;
    fclose(file);
    return success;
}

status_code xor32(const char* path_file, int* res) 
{
    FILE *file = fopen(path_file, "rb");
    if (file == NULL) return file_error;
    char byte[4];
    *res = 0;
    int bytes_cnt = 1;
    while (bytes_cnt) 
    {
        bytes_cnt = fread(byte, sizeof(char), 4, file);
        int num_32_bit = 0, offset;
        for (int i = 0; i < bytes_cnt; ++i) 
        {
            offset = 0;
            offset = (int)byte[i] << 8 * (3 - i);
            num_32_bit += offset;
        }
        *res ^= num_32_bit;
    }
    fclose(file);
    return success;
}

status_code mask(const char* path_file, char* mask_hex, int* res) 
{
    FILE *file = fopen(path_file, "rb");
    if (file == NULL) return file_error;
    int mask_num, byte;
    sscanf(mask_hex, "%x", &mask_num);
    *res = 0;
    while(fread(&byte, sizeof(unsigned int), 1, file)) 
    {
        if (mask_num == byte) (*res)++;
    }
    return success;
}

commands get_command_type(char* argv[], int argc) 
{
    if (!strcmp(argv[2], "xor8")) return XOR8;
    else if (!strcmp(argv[2], "xor32")) return XOR32;
    else if (!strcmp(argv[2], "Mask")) 
    {
        if (argc < 4) return INVALID_NUMBER_OF_ARGS;
        return MASK;
    } else return UNKNOWN;
}

int main(int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        printf("invalid number of arguments\n");
        return 1;
    }

    const char bytes[] = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6};
    int res;

    FILE* file = fopen(argv[1], "wb");
    if (file == NULL) 
    {
        printf("file open error\n");
        return 1;
    }
    fwrite(bytes, sizeof(char), sizeof(bytes) / sizeof(bytes[0]), file);
    fclose(file);

    switch (get_command_type(argv, argc)) {
        case XOR8:
            if (xor8(argv[1], &res) == file_error) 
            {
                printf("file open error\n");
                return 1;
            }
            else printf("res: %d\n", res);
            break;

        case XOR32:
            if (xor32(argv[1], &res) == file_error) 
            {
                printf("file open error\n");
                return 1;
            }
            else printf("res: %d\n", res);
            break;

        case MASK:
            if (mask(argv[1], argv[3], &res) == file_error) 
            {
                printf("file open error\n");
                return 1;
            }
            else printf("res: %d\n", res);
            break;

        case UNKNOWN:
            printf("unknown command\n");
            return 1;

        case INVALID_NUMBER_OF_ARGS:
            printf("invalid number of arguments\n");
            return 1;
    }

    return 0;
}