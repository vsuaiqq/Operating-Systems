#include <stdio.h>
#include <string.h>

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

commands get_command_type(int argc, char* argv[]) 
{
    if (!strcmp(argv[2], "xor8")) return XOR8;
    else if (!strcmp(argv[2], "xor32")) return XOR32;
    else if (!strcmp(argv[2], "Mask")) 
    {
        if (argc < 4) return INVALID_NUMBER_OF_ARGS;
        return MASK;
    } else return UNKNOWN;
}

status_code xor8(const char* path, int* res) 
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return file_error;
    char byte;
    *res = 0;
    while (fread(&byte, sizeof(char), 1, file)) *res ^= (int)byte;
    fclose(file);
    return success;
}

status_code xor32(const char* path, int* res) 
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return file_error;
    char byte[4];
    *res = 0;
    int bytes_cnt, num_32;
    do 
    {
        bytes_cnt = fread(byte, sizeof(char), 4, file);
        num_32 = 0;
        for (int i = 0; i < bytes_cnt; ++i) 
        {
            num_32 += (int)byte[i] << 8 * (3 - i);
        }
        *res ^= num_32;
    } while (bytes_cnt);
    fclose(file);
    return success;
}

status_code mask(const char* path, char* mask_hex, int* res) 
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return file_error;
    int mask_num, byte;
    sscanf(mask_hex, "%x", &mask_num);
    *res = 0;
    while(fread(&byte, sizeof(int), 1, file)) 
    {
        if (mask_num == byte) (*res)++;
    }
    return success;
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

    switch (get_command_type(argc, argv)) {
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