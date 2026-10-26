#ifndef CTH_INTERPRETER_AST_H_INCLUDED
#define CTH_INTERPRETER_AST_H_INCLUDED

#include "../common/tok.h"
#include "../common/fposT.h"
#include "../common/linking/vfastarr.h"

// stdlib includes.
#include "stdlib.h"                   // malloc, free.
#include "stdbool.h"                  // boolean types.

typedef struct cth_astS cth_astT;

typedef struct{
  cth_astT* condition;
  cth_astT** success;                 // vfastarr.
  cth_astT** failure;                 // vfastarr.
} cth_ast__ifT;

typedef struct{
  cth_astT* init;
  cth_astT* condition;
  cth_astT* update;
  cth_astT** code;                    // vfastarr.
} cth_ast__forT;

typedef struct{
  cth_astT** code;                    // vfastarr.
} cth_ast__deferT;

typedef struct{
  cth_astT* condition;
  cth_astT** code;                    // vfastarr.
  bool dowhile;                       // if true, this is a do-while statement; else it's a while.
} cth_ast__whileT;

typedef struct{
  cth_nameT id;
  cth_astT** cases;                   // vfastarr.
} cth_ast__switchT;

typedef struct{
  cth_astT* src;
} cth_ast__unaryT;

typedef struct{
  cth_astT* s0;
  cth_astT* s1;
} cth_ast__binaryT;

typedef struct{
  cth_astT** program;                 // vfastarr.
} cth_ast__programT;

// one for each possible representation an AST node takes.
typedef enum{
  CTH_AST__ID,
  CTH_AST__IF,
  CTH_AST__FOR,
  CTH_AST__DEFER,
  CTH_AST__WHILE,
  CTH_AST__SWITCH,
  CTH_AST__UNARY,
  CTH_AST__BINARY,
  CTH_AST__PROGRAM
} cth_ast_typeT;

struct cth_astS{
  cth_astT** reference;                       // ast entry pointing to this.

  cth_tokT op;
  cth_fposT line;
  cth_fposT offset;
// tagged unions in C would also reduce how much one has to write -_-...
  cth_ast_typeT type;
  union{
    cth_nameT           id;           // CTH_TOK__NAME, CTH_TOK__VALUE, CTH_TOK__CHAR, CTH_TOK__STRING.
    cth_ast__ifT        _if;          // if statements, ternary if.
    cth_ast__forT       _for;         // for statements.
    cth_ast__deferT     _defer;       // defer blocks.
    cth_ast__whileT     _while;       // while statements.
    cth_ast__switchT    _switch;      // switch statements.
    cth_ast__unaryT     _unary;       // unary ops.
    cth_ast__binaryT    _binary;      // binary ops.
    cth_ast__programT   program;      // all of the program.
  } data;
};

// node->data initialised by _whoever receives it_, although some minor work done here.
// automatically sets *reference to node when not NULL.
static cth_astT* cth_ast_init(
  cth_astT** const restrict reference, 
  const cth_tokT op, const cth_fposT line, const cth_fposT offset,
  const cth_ast_typeT type
){
  cth_astT* const restrict node = calloc(1, sizeof(*node));

  if(reference) *reference = node;
  node->reference = reference;
  node->op = op;
  node->line = line;
  node->offset = offset;
  node->type = type;

  switch(type){
    case CTH_AST__IF:
      node->data._if.success = vfastarr_init(sizeof(*node->data._if.success));
      node->data._if.failure = vfastarr_init(sizeof(*node->data._if.failure));
      break;
    case CTH_AST__FOR:
      node->data._for.code = vfastarr_init(sizeof(*node->data._for.code));
      break;
    case CTH_AST__DEFER:
      node->data._defer.code = vfastarr_init(sizeof(*node->data._defer.code));
      break;
    case CTH_AST__WHILE:
      node->data._while.code = vfastarr_init(sizeof(*node->data._while.code));
      break;
    case CTH_AST__SWITCH:
      node->data._switch.cases = vfastarr_init(sizeof(*node->data._switch.cases));
      break;
    case CTH_AST__PROGRAM:
      node->data.program.program = vfastarr_init(sizeof(*node->data.program.program));
      break;
    default: break;
  }

  return node;
}

static void cth_ast_destroy(cth_astT* const restrict node);

// destroys cth_astT* array initialised with vfastarr.
// may be passed NULL, still handles it.
static void cth_ast_destroy__array(cth_astT** const restrict arr){
  if(!arr) return;

  const size_t count = vfastarr_elcnt(arr, sizeof(*arr));
  for(size_t i = 0; i < count; i++)
    cth_ast_destroy(arr[i]);

  vfastarr_destroy(arr);
}

static void cth_ast_destroy(cth_astT* const restrict node){
  switch(node->type){
    case CTH_AST__ID: break;
    case CTH_AST__IF:
      cth_ast_destroy(node->data._if.condition);
      cth_ast_destroy__array(node->data._if.success);
      cth_ast_destroy__array(node->data._if.failure);
      break;
    case CTH_AST__FOR:
      cth_ast_destroy(node->data._for.init);
      cth_ast_destroy(node->data._for.condition);
      cth_ast_destroy(node->data._for.update);
      if(node->data._for.code) cth_ast_destroy(node->data._for.condition);
      break;
    case CTH_AST__DEFER:
      cth_ast_destroy__array(node->data._defer.code);
      break;
    case CTH_AST__WHILE:
      cth_ast_destroy(node->data._while.condition);
      cth_ast_destroy__array(node->data._while.code);
      break;
    case CTH_AST__SWITCH:
      cth_ast_destroy__array(node->data._switch.cases);
      break;
    case CTH_AST__UNARY:
      cth_ast_destroy(node->data._unary.src);
      break;
    case CTH_AST__BINARY:
      cth_ast_destroy(node->data._binary.s0);
      cth_ast_destroy(node->data._binary.s1);
      break;
    case CTH_AST__PROGRAM:
      cth_ast_destroy__array(node->data.program.program);
      break;
  }
  free(node);
}

#endif
