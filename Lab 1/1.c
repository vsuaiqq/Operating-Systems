#include <stdio.h>
#include <stdlib.h>

#define PTR_OFFSET 3
#define BYTES_TO_READ 4
#define BYTES_TO_WRITE 11

const char bytes[BYTES_TO_WRITE] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};

int main(int argc, char* argv[]) 
{
    if (argc < 2)
    {
        printf("invalid number of arguments detected\n");
        return 1;
    }

    FILE* file = fopen(argv[1], "w");
    if (file == NULL) 
    {
        printf("file open error\n");
        return 1;
    }

    if (fwrite(bytes, sizeof(char), sizeof(bytes), file) != BYTES_TO_WRITE) 
    {
        printf("file writing error\n");
        fclose(file);
        return 1;
    }

    fclose(file);

    file = fopen(argv[1], "r");
    if (file == NULL) 
    {
        printf("file open error\n");
        return 1;
    }

    char c;
    while (fread(&c, sizeof(char), 1, file)) 
    {
        printf("cur byte: %d, cur pos: %ld\n", c, ftell(file));
    }

    fclose(file);

    file = fopen(argv[1], "r");
    if (file == NULL) 
    {
        printf("file error\n");
        return 1;
    }

    fseek(file, PTR_OFFSET, SEEK_SET);

    char buff[BYTES_TO_READ];
    if (fread(buff, sizeof(unsigned char), sizeof(buff), file) != BYTES_TO_READ) 
    {
        printf("file reading error\n");
        fclose(file);
        return 1;
    }

    printf("buffer content: ");
    for (int i = 0; i < BYTES_TO_READ; ++i) 
    {
        printf("%d ", buff[i]);
    }
    printf("\n");

    fclose(file);

    return 0;
}