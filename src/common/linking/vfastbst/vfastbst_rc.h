#ifndef VFASTBST_RC_H_INCLUDED
#define VFASTBST_RC_H_INCLUDED

// insert-only binary search tree for file handling.
// uses reference-counted strings.
// exists specifically for the lth programming language.

#ifdef VFASTBST_IMPLEMENTATION
# define RC_IMPLEMENTATION
#endif

#include "../rc.h"          // reference-counted strings.
#include "../../tok.h"      // lth__representation_tokT.

// stdlib includes.
#include "stddef.h"         // for NULL, size_t.
#include "stdlib.h"         // for calloc, free.
#include "string.h"         // for strcmp.

typedef lth__representation_tokT vfastbst_rc_objT;

typedef struct vfastbst_rc_nodeS{
  rcT* key;
  vfastbst_rc_objT obj;
  struct vfastbst_rc_nodeS* leaf[2];
} vfastbst_rc_nodeT;

vfastbst_rc_nodeT* vfastbst_rc_node_init(rcT* const restrict key, const vfastbst_rc_objT obj);

// all types terminated with T and not _t to be annoyingly POSIX-compliant. fuck you POSIX why'd you do this???
typedef struct{ vfastbst_rc_nodeT* node; } vfastbst_rcT;
#define VFASTBST_RC_INIT ((vfastbst_rcT){ .node = NULL} )

void vfastbst_rc_destroy(vfastbst_rcT* const restrict bst);

void vfastbst_rc_put(vfastbst_rcT* const restrict bst, rcT* const restrict key, const vfastbst_rc_objT obj);

// returns ptr to where element would go (in which case *result == NULL), or to where element is if it exists within the tree.
vfastbst_rc_nodeT** vfastbst_rc_put_half(vfastbst_rcT* const restrict bst, rcT* const restrict key);

#endif

# ifdef VFASTBST_IMPLEMENTATION

vfastbst_rc_nodeT* vfastbst_rc_node_init(rcT* const restrict key, const vfastbst_rc_objT obj){
  vfastbst_rc_nodeT* const restrict node = calloc(1, sizeof(*node));
  node->key = rc_own(key);
  node->obj = obj;

  return node;
}

static void vfastbst_rc_node_destroy(vfastbst_rc_nodeT* const restrict node){
  if(node->leaf[0]) vfastbst_rc_node_destroy(node->leaf[0]);
  if(node->leaf[1]) vfastbst_rc_node_destroy(node->leaf[1]);

  rc_disown(node->key);
  free(node);
}

void vfastbst_rc_destroy(vfastbst_rcT* const restrict bst){
  if(bst->node) vfastbst_rc_node_destroy(bst->node);
}

void vfastbst_rc_put(vfastbst_rcT* const restrict bst, rcT* const restrict key, const vfastbst_rc_objT obj){
// inserts if tree is empty.
  vfastbst_rc_nodeT* restrict node = bst->node;
  if(!node){
    bst->node = vfastbst_rc_node_init(key, obj);
    return;
  }

  while(1){
    const int r = strcmp(node->key->str, key->str);
    if(!r)
      return;
    const size_t ch_pick = r>0;

    if(node->leaf[ch_pick])
      node = node->leaf[ch_pick];
    else{
      node->leaf[ch_pick] = vfastbst_rc_node_init(key, obj);
      return;
    }
  }
}

vfastbst_rc_nodeT** vfastbst_rc_put_half(vfastbst_rcT* const restrict bst, rcT* const restrict key){
  vfastbst_rc_nodeT *node = bst->node, **father = &bst->node;
  if(!node)
    return father;

  while(1){
    const int r = strcmp(node->key->str, key->str);
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
