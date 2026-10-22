#ifndef VFASTBST_STR_H_INCLUDED
#define VFASTBST_STR_H_INCLUDED

// insert-only binary search tree for file handling.
// uses reference-counted strings.
// exists specifically for the lth programming language.

#include "../../tok.h"      // lth__representation_tokT.

// stdlib includes.
#include "stddef.h"         // for NULL, size_t.
#include "stdlib.h"         // for calloc, free.
#include "string.h"         // for strcmp.

typedef lth__representation_tokT vfastbst_str_objT;

typedef struct vfastbst_str_nodeS{
  struct vfastbst_str_nodeS* leaf[2];
  const char* key;
  size_t s_key;
  vfastbst_str_objT obj;
} vfastbst_str_nodeT;

vfastbst_str_nodeT* vfastbst_str_node_init(const char* const restrict key, const size_t s_key, const vfastbst_str_objT obj);

// all types terminated with T and not _t to be annoyingly POSIX-compliant. fuck you POSIX why'd you do this???
typedef struct{ vfastbst_str_nodeT* node; } vfastbst_strT;
#define VFASTBST_STR_INIT ((vfastbst_strT){ .node = NULL} )

void vfastbst_str_destroy(vfastbst_strT* const restrict bst, const bool freekey);

void vfastbst_str_put(vfastbst_strT* const restrict bst, const char* const restrict key, const size_t s_key, const vfastbst_str_objT obj);

// returns ptr to where element would go (in which case *result == NULL), or to where element is if it exists within the tree.
vfastbst_str_nodeT** vfastbst_str_put_half(vfastbst_strT* const restrict bst, const char* const restrict key, const size_t s_key);

#endif

# ifdef VFASTBST_IMPLEMENTATION

vfastbst_str_nodeT* vfastbst_str_node_init(const char* const restrict key, const size_t s_key, const vfastbst_str_objT obj){
  vfastbst_str_nodeT* const restrict node = calloc(1, sizeof(*node));
  node->key = key;
  node->s_key = s_key;
  node->obj = obj;

  return node;
}

static void vfastbst_str_node_destroy(vfastbst_str_nodeT* const restrict node, const bool freekey){
  if(node->leaf[0]) vfastbst_str_node_destroy(node->leaf[0], freekey);
  if(node->leaf[1]) vfastbst_str_node_destroy(node->leaf[1], freekey);

  if(freekey) free((char*)node->key);     // very ugly.
  free(node);
}

void vfastbst_str_destroy(vfastbst_strT* const restrict bst, const bool freekey){
  if(bst->node) vfastbst_str_node_destroy(bst->node, freekey);
}

void vfastbst_str_put(vfastbst_strT* const restrict bst, const char* const restrict key, const size_t s_key, const vfastbst_str_objT obj){
// inserts if tree is empty.
  vfastbst_str_nodeT* restrict node = bst->node;
  if(!node){
    bst->node = vfastbst_str_node_init(key, s_key, obj);
    return;
  }

  while(1){
    const int r = 
      node->s_key == s_key?
        memcmp(node->key, key, s_key):
        node->s_key - s_key;
    if(!r)
      return;
    const size_t ch_pick = r>0;

    if(node->leaf[ch_pick])
      node = node->leaf[ch_pick];
    else{
      node->leaf[ch_pick] = vfastbst_str_node_init(key, s_key, obj);
      return;
    }
  }
}

vfastbst_str_nodeT** vfastbst_str_put_half(vfastbst_strT* const restrict bst, const char* const restrict key, const size_t s_key){
  vfastbst_str_nodeT *node = bst->node, **father = &bst->node;
  if(!node)
    return father;

  while(1){
    const int r = 
      node->s_key == s_key?
        memcmp(node->key, key, s_key):
        node->s_key - s_key;
    if(!r)
      return father;
    const size_t ch_pick = r>0;

    if(node->leaf[ch_pick]){
      father = &node->leaf[ch_pick];
      node = node->leaf[ch_pick];
    }
    else
      return &node->leaf[ch_pick];
  }
}

#endif
