#ifndef VFASTQUEUE_H_INCLUDED
#define VFASTQUEUE_H_INCLUDED

// VFASTQUEUE v1.2.
// WRITTEN BY 0xded.

// vfastqueue is a CC0 atomic queue implementation that I wrote.
// it is composed of a linked list with lockless atomic insert/read ops.
// at least I think they're lockless on x86, not sure about other architectures.

// this library offers:
//    fast atomic insertion & deletion, both happening contemporarily.
//    only three ops: push. pop & move (no read w/o modifying).
//    two threads cannot call vfastqueue_obj_destroy contemporarily on the same vfastqueueT context.
// usage:
//    like stb, include vfastqueue in any file of your choosing.
//    '#define VFASTQUEUE_IMPLEMENTATION' in the actual file you want the implementation to reside in, before including vfastqueue.h.

// stdlib includes.
#include "stdbool.h"              // bool type.

// HEADERS.
typedef struct vfastqueue_objS vfastqueue_objT;                                         // queue object/entry.
typedef struct vfastqueueS vfastqueueT;                                                 // queue itself.

void vfastqueue_obj_destroy(vfastqueue_objT* const obj);                                // destroys vfastqueue object.
vfastqueueT* vfastqueue_init(vfastqueueT* queue);                                       // initialises queue.
void vfastqueue_destroy(vfastqueueT* const queue, const bool freequeue);                // destroys queue.
bool vfastqueue_isempty(vfastqueueT* const queue);                                      // checks if queue is emtpy or not.
void vfastqueue_push(vfastqueueT* const restrict queue, void* const restrict elem);     // pushes element to queue.
vfastqueue_objT* vfastqueue_pop(vfastqueueT* const restrict queue);                     // pops element from queue.

// moves whole queue from src to dst.
// conditions:
//    src queue must NEVER be deleted from.
//    all ops on dst queue are done in a safe manner.
// returns true if it succeeds, false otherwise.
bool vfastqueue_move(vfastqueueT* const restrict src, vfastqueueT* const restrict dst);

#endif

// IMPLEMENTATION.
#ifdef VFASTQUEUE_IMPLEMENTATION

// stdlib includes.
#include "stddef.h"       // NULL.
#include "stdint.h"       // (u)intptr_t.
#include "stdlib.h"       // malloc, calloc.
#include "stdbool.h"      // bool type.
#include "stdatomic.h"    // atomic ops.

struct vfastqueue_objS{
  void* _Atomic next;
  void* restrict elem;
};

struct vfastqueueS{
  vfastqueue_objT* _Atomic head;      // start of queue, where elements are removed from.
  vfastqueue_objT* _Atomic tail;      // end of queue, where elements are added to.
// used for CAS lockless synchronisation ops when popping elements.
  _Atomic uintptr_t ctr;
};

static vfastqueue_objT* vfastqueue_obj_init(void* const restrict elem){
  vfastqueue_objT* const restrict obj = malloc(sizeof(*obj));

  obj->next = NULL;
  obj->elem = elem;

  return obj;
}

// to delete vfastqueue objects, we really only have to worry about when obj->next == NULL.
// deletion:
//    void* const old = atomic_exchange(obj->next, (void*)1);
//    if(old) free(obj);
// insertion:
//    void* const old = atomic_exchange(obj->next, new);
//    if(old) free(obj);

void vfastqueue_obj_destroy(vfastqueue_objT* const obj){
  void* const old = atomic_exchange(&obj->next, (void*)1);
  if(old) free(obj);
}

vfastqueueT* vfastqueue_init(vfastqueueT* queue){
  if(!queue) queue = malloc(sizeof(*queue));
// ctr is initialised to 1 so that it may never be equal to a vfastqueue_objT* so long as the vfastqueue_objT is on an aligned memory address.
// this condition is always true in C so long as vfastqueue_objT is larger than 1 char.
// it might break for large word width architectures where the compiler decides to pack the whole struct into one word.
  queue->head = NULL;
  queue->tail = NULL;
  queue->ctr = 1;
  return queue;
}

void vfastqueue_destroy(vfastqueueT* const queue, const bool freequeue){
  vfastqueue_objT* tail = queue->tail;
  while(tail){
    void* const tofree = tail;
    tail = tail->next;
    free(tofree);
  }
  if(freequeue) free(queue);
}

bool vfastqueue_isempty(vfastqueueT* const queue){
  return queue->head == queue->tail && queue->head == NULL;
}

void vfastqueue_push(vfastqueueT* const restrict queue, void* const restrict elem){
  vfastqueue_objT* const restrict new = vfastqueue_obj_init(elem);
  vfastqueue_objT* const restrict obj = atomic_exchange(&queue->tail, new);
// obj might not exist (empty queue), which is why we have to check if it does.
// we're guaranteed to be the only ones to be inserting at obj->next, but we're not guaranteed to be the only ones holding a ptr to obj.
  if(obj){
    void* const old = atomic_exchange(&obj->next, new);
    if(old) free(obj);
  }
  else atomic_store(&queue->head, new);
}

// pops one object.
vfastqueue_objT* vfastqueue_pop(vfastqueueT* const restrict queue){
// amount ctr is incremented by must be 2 so ctr is always odd & it may never be equal to any pointer.
// 'token' represents some unique value that may never conflict with any other token for the duration of the pop.
// this algorithm proves that the ABA problem can be solved with solely single-width CAS & atomic_fetch_add.
  const uintptr_t token = atomic_fetch_add(&queue->ctr, 2);
  vfastqueue_objT* old = atomic_load(&queue->head);
  if(!old) return NULL;
// weak CAS works for this algorithm, so does strong. regardless of whether weak or strong CAS is used, this is compiled to the same insn on x86.
// if queue->head == old, we replace it with our token.
// otherwise, the new queue->head is loaded & we retry.
  while(!atomic_compare_exchange_weak(&queue->head, &old, (vfastqueue_objT*)token)){
    if(!old) return NULL;       // if there's no head, we return NULL as the queue is empty.
  }
  if(!old) return NULL;

  atomic_store(&queue->head, old->next);
// swaps tail for NULL if old was the last element in the queue & we removed it,
// if, in the meanwhile, another element was added, this fails peacefully.
// strong CAS MUST be used or else this won't work; this cannot fail sporadically.
// sets old->next to some value so that when an entry with next field NULL is being deleted it only means that it was the head & it's being populated.
  if(!old->next){
    if(atomic_compare_exchange_strong(&queue->tail, &old, NULL))
      old->next = old;
  }
  return old;
}

bool vfastqueue_move(vfastqueueT* const restrict src, vfastqueueT* const restrict dst){
  vfastqueue_objT* hd = atomic_load(&src->head);
  if(!hd) return false;
// idk if these MUST be atomic stores; to be safe...
  atomic_store(&dst->head, hd);
  atomic_store(&dst->tail, atomic_exchange(&src->tail, NULL));
  return true;
}

#endif
