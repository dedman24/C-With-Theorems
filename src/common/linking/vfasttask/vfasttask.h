// vfasttask is a simple CC0 C11 (for _Atomic, otherwise all features are C99) library that simplifies task handling for multithreaded applications.
// it allows one to abstract away task dispatch.

// stdlib includes.
#include "stddef.h"         // NULL, size_t.
#include "stdint.h"         // uintx types.
#include "stdlib.h"         // malloc, free.
#include "stdbool.h"        // bool, true, false.
#include "stdatomic.h"      // fetch_add, fetch_sub, compare_exchange.

// posix includes.
#include "pthread.h"        // pthread_create, pthread_t.

#ifndef VFASTTASK_H_INCLUDED
#define VFASTTASK_H_INCLUDED

typedef 
  #ifndef VFASTTASK_TYPE
    uint16_t
  #else
    VFASTTASK_TYPE
  #endif
      vfasttask_threadctrT;

typedef struct vfasttaskS vfasttaskT;

// VFASTTASK API is composed of these two functions.

// starts execution on vfasttask.
//  'fn'              ~ function that should be executed in parallel. called through a wrapper function.
//  'maxthreads'      ~ the maximum number of threads vfasttask can issue.
//  'data'            ~ data that should be given to the executing function, must be dynamically allocated.
//  'createNewThread' ~ set to true if a new executing thread for this given task should be created, false if it should be the same thread that called the function. 
void vfasttask_start(void (*fn)(void* const data, vfasttaskT* const restrict task), const vfasttask_threadctrT maxthreads, void* const data, const bool createNewThread);

// pushes data to queue, possibly creates new thread/s.
//  'task' ~ task context.
//  'data' ~ data to push, must be dynamically allocated.
//  'fn'   ~ function that should be executed in parallel were it necessary
void vfasttask_push(void (* const fn)(void* const data, vfasttaskT* const restrict task), void* const restrict data, vfasttaskT* const restrict task);

#endif

#ifdef VFASTTASK_IMPLEMENTATION

#define VFASTQUEUE_IMPLEMENTATION
#include "vfastqueue.h"

struct vfasttaskS{
  vfastqueueT queue;                            // lockless queue used to access elements.
  vfasttask_threadctrT maxthreads;
  _Atomic vfasttask_threadctrT curthreads;
  _Atomic vfasttask_threadctrT ref;
};

static vfasttaskT* vfasttask_init(const vfasttask_threadctrT maxthreads){
  vfasttaskT* const restrict task = malloc(sizeof(*task));

  vfastqueue_init(&task->queue);
  task->maxthreads = maxthreads;
  task->curthreads = 1;                         // HAS to be initialised to 1.
  task->ref = 0;                                // NOTE: be very careful about this value, as depending on the code it can be either 0 or 1.

  return task;
}

static vfasttaskT* vfasttask_own(vfasttaskT* const restrict task){
  atomic_fetch_add(&task->ref, 1);
  return task;
}

static vfasttaskT* vfasttask_destroy(vfasttaskT* const restrict task){
  atomic_fetch_sub(&task->curthreads, 1);
  if(atomic_fetch_sub(&task->ref, 1) == 1){       // nobody else owns vfasttaskT, therefore we can destroy it.
    vfastqueue_destroy(&task->queue, false);
    free(task);
  }
}

typedef struct{
  void (* fn)(void* const data, vfasttaskT* const restrict task);
  void* restrict data;
  vfasttaskT* restrict task;
} vfasttask_argT;

static vfasttask_argT* vfasttask_arg_init(void (* const fn)(void* const data, vfasttaskT* const restrict task), void* const data, vfasttaskT* const restrict task){
  vfasttask_argT* const restrict arg = malloc(sizeof(*arg));

  arg->fn = fn;
  arg->data = data;
  arg->task = vfasttask_own(task);

  return arg;
}

static void vfasttask_arg_destroy(vfasttask_argT* const restrict arg){
  vfasttask_destroy(arg->task);
  free(arg->data);
  free(arg);
}

// helper function that helps dispatch vfasttask calls.
static void* vfasttask__helper(void* hidden__arg){
  vfasttask_argT* const arg = hidden__arg;

  void* data = arg->data;  
  while(1){
    arg->fn(data, arg->task);
    vfastqueue_objT* const restrict obj = vfastqueue_pop(&arg->task->queue);
    if(!obj) break;
    
    data = obj->elem;
    vfastqueue_obj_destroy(obj);
  } 

  vfasttask_arg_destroy(arg);
  return NULL;
}

void vfasttask_start(void (*fn)(void* const data, vfasttaskT* const restrict task), const vfasttask_threadctrT maxthreads, void* const data, const bool createNewThread){
  vfasttaskT* const restrict task = vfasttask_init(maxthreads);
  vfasttask_argT* const restrict args = vfasttask_arg_init(fn, data, task);

  if(createNewThread){
    pthread_t threadid;
    pthread_create(&threadid, NULL, vfasttask__helper, args);
  }
  else vfasttask__helper(args);
}

void vfasttask_push(void (* const fn)(void* const data, vfasttaskT* const restrict task), void* const restrict data, vfasttaskT* const restrict task){
// load task->curthreads atomically.
// try CAS(task->curthreads, our curthreads, our curthreads + 1);
// if it fails:
//   if our curthreads == maxthreads, push to queue.      (* this means that the maximum number of threads has been allocated *)
//   else retry.
// if it succeeds:
//   create new thread.
  bool result;
  do{
    vfasttask_threadctrT expected = atomic_load(&task->curthreads);
    if(expected == task->maxthreads){ vfastqueue_push(&task->queue, data); return; }
    result = atomic_compare_exchange_strong(&task->curthreads, &expected, expected + 1);
    if(expected == task->maxthreads){ vfastqueue_push(&task->queue, data); return; }
//  else retry;
  } while(!result);

  vfasttask_argT* const restrict args = vfasttask_arg_init(fn, data, task);

  pthread_t threadid;
  if(pthread_create(&threadid, NULL, vfasttask__helper, args) != 0){   // handles errors by just pushing data to queue. 
    vfasttask_arg_destroy(args);
    vfastqueue_push(&task->queue, data);
  }
}

#endif
