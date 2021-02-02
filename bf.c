#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bf.h"

#define LINE_BUFFER_SIZE (256)
#define MAX_TEXT_SIZE (16 * 1024)
#define MAX_DATA_SIZE (32 * 1024)

typedef struct
{
    uint8_t *data;
    uint8_t *text;
    uint32_t text_size;
}BF_Interpreter;

static const char bf_symbols[] = {',', '.', '+', '-', '[', ']', '>', '<', 0};

static BF_Interpreter __attribute__((aligned(64))) it;

static void iterate_examples(const char *examples_file)
{
    if(NULL == examples_file)
    {
        printf("NULL pointer parameter\n");
    }
    FILE *file = fopen(examples_file, "r");
    if(NULL == file)
    {
        printf("Error opening file %s\n", examples_file);
    }
    else
    {
        char current_line[LINE_BUFFER_SIZE];
        while(NULL != fgets(current_line, LINE_BUFFER_SIZE, file))
        {
            size_t length = strlen(current_line);
            if(length > 0)
            {
                current_line[length - 1] = 0;
            }
            if(0 != current_line[0])
            {
                if(0 != load_bf_file(current_line))
                {
                    exit(-1);
                }
                if(0 != interpret_bf())
                {
                    exit(-1);
                }
            }
        }
        fclose(file);
    }
}

int32_t interpret_bf(void)
{
    if(0 == it.text_size)
    {
        free(it.text);
        free(it.data);
        return -1;
    }
    char c = EOF;
    for(size_t pc = 0, dp = 0, mb = 0; pc < it.text_size; ++pc)
    {
        switch(it.text[pc])
        {
            case '+':
                it.data[dp]++;
                break;
            case '-':
                it.data[dp]--;
                break;
            case '<':
            case '>':
                dp += 1 + it.text[pc] - '>';
                if(dp > MAX_DATA_SIZE)
                {
                    printf("data pointer exceeded max size\n");
                }
                break;
            case ',':
                while(EOF == c)
                {
                    printf("\nInsert a valid ASCII character: ");
                    c = fgetc(stdin);
                }
                it.data[dp] = (uint8_t)c;
                break;
            case '.':
                printf("%c", it.data[dp]);
                break;
            case '[':
                if(0 == it.data[dp])
                {
                    mb = 1;
                    while(0 != mb)
                    {
                        pc++;
                        if(('[' == it.text[pc]) || (']' == it.text[pc]))
                        {
                            mb += 1 + '[' - it.text[pc];
                        }
                    }
                }
                break;
            case ']':
                if(0 != it.data[dp])
                {
                    mb = 1;
                    while(0 != mb)
                    {
                        pc--;
                        if(('[' == it.text[pc]) || (']' == it.text[pc]))
                        {
                            mb += 1 + it.text[pc] - ']';
                        }
                    }
                }
                break;
            default:
                printf("how did I end up here?\n");
                break;
        }
    }
    free(it.text);
    free(it.data);
    return 0;
}

int32_t load_bf_file(const char *bf_file)
{
    if(NULL == bf_file)
    {
        printf("NULL pointer parameter\n");
        return -1;
    }
    FILE *file = fopen(bf_file, "r");
    if(NULL == file)
    {
        printf("Error opening file %s\n", bf_file);
        return -1;
    }
    else
    {
        int current_char;
        size_t iteration = 0;
        it.text = (uint8_t *)malloc(MAX_TEXT_SIZE * sizeof(uint8_t));
        if(NULL == it.text)
        {
            printf("Not enough heap to allocate memory for text\n");
            return -1;
        }
        it.data = (uint8_t *)malloc(MAX_DATA_SIZE * sizeof(uint8_t));
        if(NULL == it.data)
        {

            printf("Not enough heap to allocate memory for data\n");
            return -1;
        }
        while(EOF != (current_char = fgetc(file)))
        {
            if(NULL != strchr(bf_symbols, current_char))
            {
                it.text[iteration++] = (uint8_t)current_char;
            }
        }
        it.text_size = iteration;
        printf("Running the following BF program:\n");
        for(size_t i = 0; i < iteration; ++i)
        {
            printf("%c", it.text[i]);
        }
        printf("\n");
        fclose(file);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if(2 == argc)
    {
        iterate_examples(argv[1]);
    }
    else
    {
        printf("Incorrect number of arguments\n");
    }
    return 0;
}

