#include "io.h"
#include "code.h"
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <memory.h>

#define BLOCK 4096     
#define BYTE 8   
#define LIMIT 40000  
#define MAGIC 0x8badbeef


uint8_t symB[BLOCK] = {0};
uint8_t bitB[BLOCK] = {0};

int allB;
int symI = 0;
int totalS = 0;
int bitIn = 0;

static void save(int index, bool bit);

static bool release(int index);


int read_bytes(int infile, uint8_t *buf,  int to_read) 
{

    int length = 0;
    for (;length < to_read;) 
    {
        int a = to_read - length;
        int detail = read(infile, &buf[length], a);

        if (detail <= 0)
        {


            break;
        }
        length = length + detail;
    }

    return length;
}

int write_bytes(int outfile, uint8_t *buf, int to_write) 
{

    int wLength = 0;
    for(;wLength < to_write;) 
    {
        int b = to_write - wLength;
        int detail = write( outfile, &buf[wLength], b);

        if ( detail < 0) 
        {

            break;
        }

        wLength = wLength + detail;
    }

    return wLength;
}


void read_header(int infile, FileHeader *header) 
{

    int detail = read_bytes(infile, (uint8_t*)header, sizeof(FileHeader));

    if (detail != sizeof(FileHeader)) 
    {
        printf("read_h wrong");
        return;
        //debug
    }
}


void write_header(int outfile, FileHeader *header) 
{

    int detail = write_bytes(outfile, (uint8_t*)header, sizeof(FileHeader));

    if (detail != sizeof(FileHeader)) 
    {
        printf("write_h wrong");
        return;
    }
}

bool read_sym(int infile, uint8_t *byte) 
{

    if (symI < totalS) 
    {
        symI += 1;
        *byte = symB[ symI ];
        return true;
    }

    memset(symB, 0, BLOCK);
    totalS = read_bytes(infile, symB, BLOCK);
    
    symI = 0;
    symI += 1;
    *byte = symB[ symI ];
    return true;
    if (totalS <= 0) 
    {
        return false;
    }
}

void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bit_len) 
{

    uint32_t a = 0;
    uint32_t totalN = BYTE + bit_len;
    uint32_t firstB = LIMIT - bitIn;
    uint32_t data = (sym << bit_len) | code;
    
    for (; (a < totalN) && (a < firstB); ) 
    {
        save( bitIn += 1, data & (1u << allB));
        a += 1;
    }
    if (a < firstB) 
    {
 
        return;
    }
    int detail = write_bytes(outfile, bitB, BLOCK);

    if ( detail != BLOCK) 
    {
        printf("buffer_pair wrong");
        return;
    }

    bitIn = 0;

    for (; a < totalN;) 
    {

        save( bitIn += 1, data & (1u << a));
        a += 1;
    }
}


void flush_pairs(int outfile) 
{
    if (bitIn != 0) 
    {
        int length = write(outfile, bitB, (bitIn - 1) / BYTE + 1);

        if (length != (bitIn - 1) / BYTE + 1) 
        {
            printf("flush_pairs wrong");
        }
    
        memset(bitB, 0, BLOCK);
  
        bitIn = 0;
    }
    return;
}




bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bit_len) 
{
    uint32_t totalN = BYTE + bit_len;


    uint32_t firstB = allB - bitIn;

    uint32_t data = 0;

 
    uint32_t b = 0;
    for (; b < totalN && b < firstB;) 
    {
       
        data |= (release(bitIn) << b);
        bitIn += 1;
        b += 1;
    }


    if (!(b < firstB)) 
    {
        memset(bitB, 0, BLOCK);

        allB = read_bytes(infile, bitB, BLOCK);

        if (allB >0) 
        {
            allB *= BYTE;
            bitIn = 0;
            for (; b < totalN;) 
            {
                data |= (release(bitIn) << b);
                bitIn += 1;
                b += 1;
            }
        }
        else
        {
            return false;
        }
        
    }


    *code = data & ~(0xffffffff << bit_len);

    if (*code != STOP_CODE) 
    {
        *sym = (data >> bit_len) & 0xff;

        return true;
    }
    else
    {
        return false;
    }
    
}

void buffer_word(int outfile, Word *w) 
{
    uint32_t i = 0;
    
    for (; i < (uint32_t)(BLOCK - symI) && i < w->len; ) 
    {

        symB[symI++] = w->syms[i];
        i += 1;
    }


    if (i < (uint32_t)(BLOCK - symI)) 
    {
        return;
    }
    int length = write_bytes(outfile, symB, BLOCK);

    if (length != BLOCK) 
    {
        printf("buffer_word wrong");
    }

    memset(symB, 0, BLOCK);

    symI = 0;

    for (; i < w->len; ++i) 
    {
        symB[symI++] = w->syms[i];
    }
}

void flush_words(int outfile) 
{
    if (symI == 0) 
    {
        printf("flush_word wrong");
        return;
    }
    int length = write_bytes(outfile, symB, symI);

    if (length != symI) 
    {
        printf("flush_words wrong");
    }


    symI = 0;
}

//https://stackoverflow.com/questions/2128442/what-does-1u-x-do
static void save(int index, bool bit) 
{
    int pos = index / BYTE;
    if (bit) 
    {
        bitB[pos] |= (1u << (index % BYTE));
    }  
    else 
    {
        bitB[pos] &= ~(1u << (index % BYTE));
    }
}



static bool release(int index) 
{
    int pos = index / BYTE;
  
    bool bit = bitB[pos] & (1u << (index % BYTE));

    return bit;
}
