#include "code.h"
#include "word.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>



Word *word_create(uint8_t *syms, uint32_t len) 
{

    Word* word = malloc(sizeof(Word));
    if (word == NULL) 
    {

        perror(" word_create failed ");
        exit(EXIT_FAILURE);
    }

    word->syms = NULL;
    word->len = len;

    if (len <= 0) 
    {
        return word;
    }

    word->syms = malloc(sizeof(uint8_t) * len);

    if (word->syms == NULL) 
    {

        free(word);
        perror(" word_create syms failed ");
        exit(EXIT_FAILURE);

    }

    memcpy(word->syms, syms, len);

    return word;
}

Word *word_append_sym(Word *w, uint8_t sym) 
{

    Word* word = malloc(sizeof(Word));

    if (word == NULL) 
    {

        perror(" word_append syms failed ");
        exit(EXIT_FAILURE);
    }

    if (word != NULL)
    {
        //debug
        word->len = w->len + 1;

        word->syms = malloc( sizeof(uint8_t) * word->len);

        if (!word->syms) 
        {
            free(word);
            perror(" word_append malloc syms failed ");
            exit(EXIT_FAILURE);
        }
  
        memcpy(word->syms, w->syms, w->len);
  
        word->syms[w->len] = sym;

    }

    return word;
}


void word_delete(Word *w) 
{
    if (w != NULL) 
    {
        free(w->syms);
   
        free(w);
    }
}


WordTable *wt_create(void) 
{

    WordTable* word = malloc(MAX_CODE * sizeof(WordTable));
    if (word == NULL) 
    {
        perror("wt_create malloc failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        //debug
        Word*temp = word_create(NULL, 0);
        memset(word, 0, MAX_CODE * sizeof(WordTable));
        word[EMPTY_CODE] = temp;
  
    }
    

    return word;
}


void wt_reset(WordTable *wt) 
{

    int a = START_CODE;
    for ( ;a < MAX_CODE;) 
    {

        word_delete(wt[a]);
        wt[a] = NULL;
        a +=1;
    }
}

void wt_delete(WordTable *wt)
{

    wt_reset(wt);
    word_delete(wt[EMPTY_CODE]);
    free(wt);
}
