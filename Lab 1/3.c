#include <stdio.h>

void copy_file(FILE* input_file, FILE* output_file) 
{
    char c;
    while (fread(&c, sizeof(char), 1, input_file)) 
    {
        fwrite(&c, sizeof(char), 1, output_file);
    }
}

int main(int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        printf("invalid number of arguments\n");
        return 1;
    }

    FILE* input_file = fopen(argv[1], "rb");
    FILE* output_file = fopen(argv[2], "wb");

    if (input_file == NULL || output_file == NULL) 
    {
        fclose(input_file), fclose(output_file);

        printf("file open error\n");
        return 1;
    }

    copy_file(input_file, output_file);

    fclose(input_file), fclose(output_file);

    return 0;
}