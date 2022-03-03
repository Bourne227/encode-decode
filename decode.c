#define OPTIONS "hvi:o:"
#include "io.h"
#include "code.h"
#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#define MAGIC 0x8badbeef

static void decode(int input, int output);

static uint8_t bits(uint16_t code);

static uint8_t bits(uint16_t code)
{

    return ((uint8_t)floor(log2(code))) + 1;
}

int main(int argc, char *argv[])
{
    int x;

    bool condi = false;

    char *inputN = NULL;

    char *outputN = NULL;

    while ((x = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (x)
        {
            case 'v':

                condi = true;
                break;

            case 'o':

                outputN = optarg;
                break;

            case 'i':


                inputN = optarg;
                break;

            case 'h':
    
                exit(EXIT_SUCCESS);
            
            default:
                exit(EXIT_FAILURE);
        }
    }
    if (optind < argc)
    {
        exit(EXIT_FAILURE);
    }

    int input = STDIN_FILENO;
    int output = STDOUT_FILENO;

    if (inputN != NULL)
    {
        input = open(inputN, O_RDONLY);

        if (input < 0)
        {
            perror("decode: failed");
            exit(EXIT_FAILURE);
        }
    }

    FileHeader header;
    read_header(input, &header);

    if (header.magic != MAGIC)
    {
        fprintf(stderr, "Invalid magic number\n");
        close(input);
        exit(EXIT_FAILURE);
    }

    if (outputN != NULL)
    {
        output = open(outputN, O_WRONLY | O_CREAT | O_TRUNC, header.protection);

        if (output < 0)
        {
            close(input);
            perror(" open output file failed");
            exit(EXIT_FAILURE);
        }
    }

   decode(input, output);

   if (condi)
   {
       struct stat statsIn;
       struct stat statsOut;

       fstat(input, &statsIn);
       fstat(output, &statsOut);

       printf("Compressed file size: %lld bytes\n", statsIn.st_size);
       printf("Uncompressed file size: %lld bytes\n", statsOut.st_size);
       printf("Compression ratio: %.2f%%\n",
           100 * (1 - (( double )statsIn.st_size / statsOut.st_size)));
    }

    close(input);
    close(output);

    return 0;
}

static void decode(int input, int output)
{
    WordTable *decodeT = wt_create();
    uint8_t Csym = 0;
    uint16_t Ccode = 0;
    uint16_t Ncode = START_CODE;

    for (; read_pair(input, &Ccode, &Csym, bits(Ncode));)
    {

        decodeT[Ncode] = word_append_sym(decodeT[Ccode], Csym);

        buffer_word(output, decodeT[Ncode]);

        ++Ncode;

        if (Ncode == MAX_CODE)
        {
            wt_reset(decodeT);
            Ncode = START_CODE;
        }
    }

    flush_words(output);

    wt_delete(decodeT);
}
