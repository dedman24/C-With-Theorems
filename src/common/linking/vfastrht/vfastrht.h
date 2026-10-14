#ifndef VFASTRHT_H_INCLUDED
#define VFASTRHT_H_INCLUDED

// SPECIAL VFASTRHT SPECIFICALLY FOR USE IN THIS PROJECT.

// vfastrht is a fast data structure like bsts or b-trees. it is faster than both, however, when considering massive amounts of entries.
// this is because it indexes into an array once per byte for at most however many bytes are in the key.
// search, insert & deletion of individual elements are all O(logn).
// the algorithm is most efficient for when all values inserted have large, random keys. 
// it is most inefficient when all keys are very similar except for a couple of bits towards the end of the key.

#include "../../tok.h"                  // satisfiable__representation_tokT.

// stdlib includes.
#include "stddef.h"                     // for size_t, NULL.
#include "stdlib.h"                     // for calloc, free.
#include "string.h"                     // for memcmp.
#include "stdbool.h"                    // for boolean type.

typedef char* vfastrht_keyT;
typedef satisfiable__representation_tokT vfastrht_objT;
typedef struct vfastrhtS{
    struct vfastrhtS* restrict under;
    vfastrht_keyT key;
    size_t s_key;
    vfastrht_objT obj;
} vfastrhtT;

#define VFASTRHT_OBJ_NULL SATISFIABLE_TOK__MAX;

vfastrhtT* vfastrht_table_init();                                                                               // initialises table.
void vfastrht_table_destroy(vfastrhtT* const restrict table);                                                   // destroys table.
bool vfastrht_put(vfastrhtT* restrict table, vfastrht_keyT key, const size_t s_key, const vfastrht_objT obj);   // puts entry in table, returns true if successful.
vfastrhtT* vfastrht_put_half(vfastrhtT* restrict table, vfastrht_keyT key, const size_t s_key);                 // checks if entry can be put in table, returns table object if successful.
bool vfastrht_search(vfastrhtT* restrict table, vfastrht_keyT key, const size_t s_key);                         // searches entry within table, returns true if successful.
vfastrht_objT vfastrht_search_elem(vfastrhtT* restrict table, vfastrht_keyT key, const size_t s_key);           // searches entry within table, returns obj if successful.

#endif

#ifdef VFASTRHT_IMPLEMENTATION

// 256 elements/table so that the key can be indexed as a byte array.
#define VFASTRHT_TABLE_SIZE 256

vfastrhtT* vfastrht_table_init(){
    return (vfastrhtT*)calloc(VFASTRHT_TABLE_SIZE, sizeof(vfastrhtT));
}

void vfastrht_table_destroy(vfastrhtT* const restrict table){
    for(size_t i = 0; i < VFASTRHT_TABLE_SIZE; i++)
        if(table[i].under) vfastrht_table_destroy(table[i].under);
    free(table);
}

static size_t vfastrht__idx(vfastrht_keyT key, unsigned short r){
    return ((unsigned char*)key)[r];
}

bool vfastrht_put(vfastrhtT* restrict table, vfastrht_keyT key, const size_t s_key, const vfastrht_objT obj){
    unsigned short r = 0;
    do{
        const size_t idx = vfastrht__idx(key, r);
        if(!table[idx].key){
            table[idx].key = key;
            table[idx].s_key = s_key;
            table[idx].obj = obj;
            return true;
        }
        else if(table[idx].s_key != s_key || !memcmp(table[idx].key, key, s_key))
            return false;

        if(!table[idx].under && r != s_key - 1)
            table[idx].under = vfastrht_table_init();

        table = table[idx].under;
        r++;
    } while(r < s_key);

    return false;
}

vfastrhtT* vfastrht_put_half(vfastrhtT* restrict table, vfastrht_keyT key, const size_t s_key){
    unsigned short r = 0;
    do{
        const size_t idx = vfastrht__idx(key, r);
        if(!table[idx].key)
            return &table[idx];
        else if(table[idx].s_key == s_key && !memcmp(table[idx].key, key, s_key))
            return &table[idx];

        if(!table[idx].under && r != s_key - 1)
            table[idx].under = vfastrht_table_init();

        table = table[idx].under;
        r++;
    } while(r < s_key);

    return NULL;
}

bool vfastrht_search(vfastrhtT* restrict table, vfastrht_keyT key, const size_t s_key){
    unsigned short r = 0;

    do{
        const size_t idx = vfastrht__idx(key, r);
        if(table[idx].key && table[idx].s_key == s_key && !memcmp(table[idx].key, key, s_key))
            return true;
        if(!table[idx].under)
            return false;
        table = table[idx].under;

        r++;
    } while(r < s_key);

    return false;
}

vfastrht_objT vfastrht_search_elem(vfastrhtT* restrict table, vfastrht_keyT key, const size_t s_key){
    unsigned short r = 0;

    do{
        const size_t idx = vfastrht__idx(key, r);
        if(table[idx].key && table[idx].s_key == s_key && !memcmp(table[idx].key, key, s_key))
            return table[idx].obj;
        if(!table[idx].under)
            return VFASTRHT_OBJ_NULL;
        table = table[idx].under;

        r++;
    } while(r < s_key);

    return VFASTRHT_OBJ_NULL;
}

#endif
