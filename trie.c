#include "code.h"
#include "trie.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>



TrieNode *trie_node_create(uint16_t index) 
{

    TrieNode* node = malloc(sizeof(TrieNode));
    if (node == NULL) 
    {

        perror(" malloc failed");
        exit(EXIT_FAILURE);
    }

    memset(node, 0, sizeof(TrieNode));
  
    node->code = index;

    return node;
}



void trie_node_delete(TrieNode *n) 
{

    free(n);
}

TrieNode *trie_create(void)
{

    return trie_node_create(EMPTY_CODE);
}


void trie_reset(TrieNode *root) 
{

    if (root == NULL) 
    {

        return;
    }
    int a = 0;
    for (; a < ALPHABET;) 
    {
        
        trie_delete(root->children[a]);
        root->children[a] = NULL;
        a+=1;
    }
}


void trie_delete(TrieNode *n) 
{

    trie_reset(n);
 
    trie_node_delete(n);
}


TrieNode *trie_step(TrieNode *n, uint8_t sym) 
{
    
    return n->children[sym];
}
