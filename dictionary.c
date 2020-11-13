// Implements a dictionary's functionality

/*
the hash algorithm
http://www.cse.yorku.ca/~oz/hash.html
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "dictionary.h"
#include <ctype.h>
#include <strings.h>

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table
const unsigned int N = 100000;

// word counter to determine number of words, incremented in load and used in size.
int WORD_COUNT = 0;
// array of hashes that have been made, populated in load, for use in unload
unsigned int HASHES[N];

// Hash table
node *table[N];

// prototype
node *find_word(FILE *fread_ptr, char *lett);
void store_word(node *n, unsigned int hash);
void lowerstr(const char *word, char *wordlo);
void llist_free(node *head);

// Loads dictionary into memory (hash table), returning true if successful else false
bool load(const char *dictionary)
{
    // initiate file pointer
    FILE *dic_read = fopen(dictionary, "r");
    if (dic_read == NULL)
    {
        printf("dic is NULL\n");
        return false;
    }

    // index for the array of hashes
    int h_i = 0;

    // loop until file reaches end
    char lett = getc(dic_read);
    while (lett != EOF)
    {
        // for every line
        if (lett != '\n')
        {
            node *n = find_word(dic_read, &lett);
            if (n == NULL)
            {
                printf("n=null");
                return false;
            }

            unsigned int hash_num = hash(n->word);
            // printf("word to be hashed:%s|hash num:%d \n", n->word, hash_num);

            // store hash
            // if not stored
            if (table[hash_num] == NULL)
            {
                // store the hash
                HASHES[h_i] = hash_num;
                h_i++;
            }

            store_word(n, hash_num);

            WORD_COUNT++;
        }
        // else it is new line, skip new line character
        else
        {
            lett = getc(dic_read);
        }
    }
    // terminate the HASHES array with a -1 number
    HASHES[h_i] = -1;
    printf("hashes array index after loading: %d\n", h_i);
    fclose(dic_read);
    printf("loaded dic\n");
    return true;
}


// find a word and create a node for that word
node *find_word(FILE *fread_ptr, char *lett)
{
    node *n = malloc(sizeof(node));
    n->next = NULL;

    // read each letter to extract word
    int l_i = 0; // letter index
    while (*lett != '\n' && *lett != EOF)
    {
        n->word[l_i] = *lett;
        l_i++;
        *lett = getc(fread_ptr);
    }
    // terminating character
    n->word[l_i] = '\0';

    return n;
}

// store word in the right hash bucket
void store_word(node *n, unsigned int hash_num)
{
    // link the table to a node
    if (table[hash_num] == NULL)
    {
        table[hash_num] = n;
    }
    else // insert the node
    {
        n->next = table[hash_num]->next;
        table[hash_num]->next = n;
    }
}


// Hashes word to a number
unsigned int hash(const char *word)
{
    // the hash code
    unsigned long hash = 5381;
    int c;
    while ((c = *word++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    // change to positive number and reduce range:
    hash = hash % N;
    return hash;
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    node *temp= NULL;
    // make all lower case
    char wordlo[LENGTH + 1];
    lowerstr(word, wordlo);

    // hash the lower cased word
    unsigned int hash_num = hash(wordlo);

    // set temp for parsing nodes
    for (temp = table[hash_num]; temp != NULL; temp = temp->next)
    {
        if (strcasecmp(word, temp->word) == 0)
        {
            return true;
        }
    }
    return false;
}

void lowerstr(const char *word, char *wordlo)
{
    int i = 0;
    while (word[i] != '\0')
    {
        wordlo[i] = tolower(word[i]);
        i++;
    }
    wordlo[i] = '\0';
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    // TODO
    return WORD_COUNT;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    // look at each hash in the array
    int i = 0;
    while (HASHES[i] != -1)
    {
        // printf("f.");
        if (table[ HASHES[i] ] == NULL)
        {
            printf("null hash encounter in unload");
            return false;
        }

        llist_free(table[ HASHES[i] ]);
        // printf("free hash loc: %d",  HASHES[i]);
        i++;
    }
    if (HASHES[i] == -1)
    {
        return true;
    }
    return false;
}

void llist_free(node *head)
{
    node *tmp = head;
    node *tmp2 = head->next;
    while (tmp2 != NULL)
    {
        free(tmp);
        tmp = tmp2;
        tmp2 = tmp2->next;
    }
    free(tmp);
    return;
}

