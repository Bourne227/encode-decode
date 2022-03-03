#define OPTIONS "hvi:o:"
#include "io.h"
#include "code.h"
#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <memory.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <math.h>
#define MAGIC 0x8badbeef


static void encode(int input, int output);


static uint8_t bits(uint16_t code);

static uint8_t bits(uint16_t code) 
{
    return ((uint8_t) floor(log2(code))) + 1;
}



int main(int argc, char* argv[]) 
{
    int x;
  
    char* inputN = NULL;

    char* outputN = NULL;


    bool condi = false;
  
    while (( x = getopt (argc , argv , OPTIONS)) != -1) 
    {
        switch (x) 
        {
            case 'v':
       
                condi = true;
                break;
            
            case 'i': 
                inputN = optarg;
                break;
            
            case 'o': 
       
                outputN = optarg;
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
            perror("encode: failed");
            exit(EXIT_FAILURE);
        }
    }
    
    if (outputN != NULL) 
    {
        output = open(outputN, O_WRONLY| O_CREAT  | O_TRUNC);
   
        if (output < 0) 
        {
            close(input);
            perror("encode: failed");
            exit(EXIT_FAILURE);
        }
    }
  
    encode(input, output);

    if (condi) 
    {

        struct stat statsIn;
        struct stat statsOut;

        fstat(input, &statsIn);
        fstat(output, &statsOut);

        printf("Compressed file size: %lld bytes\n", statsOut.st_size );
        printf("Uncompressed file size: %lld bytes\n", statsIn.st_size );
        printf("Compression ratio: %.2f%%\n",
            100 * ( 1 - (( double ) statsOut.st_size / statsIn.st_size )));
    }

 
    close(input);
    close(output);
    return 0;
}

static void encode(int input, int output) 
{  
    
    FileHeader header;
    header.magic = MAGIC;

    struct stat st;
    fstat(input, &st);
    header.protection = st.st_mode;

    write_header(output, &header);


    TrieNode* root = trie_create();

    TrieNode* Cnode = root;
    TrieNode* Pnode = NULL;
    uint16_t Ncode = START_CODE;
    uint8_t Psym = 0;
    uint8_t Csym = 0;
  
    for (;read_sym(input, &Csym);) 
    {
        
        TrieNode* nextN = trie_step(Cnode, Csym);

        if (nextN == NULL) 
        {
            buffer_pair(output, Cnode->code, Csym, bits(Ncode));

            Cnode->children[Csym] = trie_node_create(Ncode);
    
            Cnode = root;

            ++Ncode;
        } 
        else 
        {
 
            Pnode = Cnode;
            Cnode = nextN;
        }


        if (Ncode == MAX_CODE) 
        {

            trie_reset(root);
            Cnode = root;
            Ncode = START_CODE;
        }

  
        Psym = Csym;
    }

    if (Cnode != root && Pnode) 
    {

        buffer_pair(output, Pnode->code, Psym, bits(Ncode));
        Ncode = (Ncode + 1) % MAX_CODE;
    }

 
    buffer_pair(output, STOP_CODE, 0, bits(Ncode));

    flush_pairs(output);

    trie_delete(root);
}




