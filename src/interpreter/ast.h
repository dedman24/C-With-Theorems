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
  cth_astT* s0;                       // right argument.
  cth_astT* s1;                       // left argument.
} cth_ast__binaryT;

typedef struct{
  cth_astT* fun;                      // function to call.
  cth_astT** args;                    // vfastarr, arguments to function.
} cth_ast__funcallT;

typedef struct{
  cth_astT** program;                 // vfastarr.
} cth_ast__programT;

typedef struct{
  cth_astT* type;
  cth_astT** expr;                    // vfastarr, expression variables should evaluate to.
  cth_nameT* vars;                    // vfastarr, variable IDs themselves.
} cth_ast__declarationT;

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
  CTH_AST__FUNCALL,
  CTH_AST__PROGRAM,
  CTH_AST__DECLARATION
} cth_ast_typeT;

struct cth_astS{
  cth_tokT op;
  cth_fposT line;
  cth_fposT offset;
// tagged unions in C would also reduce how much one has to write -_-...
  cth_ast_typeT type;
  union{
    cth_nameT             id;           // CTH_TOK__NAME, CTH_TOK__VALUE, CTH_TOK__CHAR, CTH_TOK__STRING.
    cth_ast__ifT          _if;          // if statements, ternary if.
    cth_ast__forT         _for;         // for statements.
    cth_ast__deferT       _defer;       // defer blocks.
    cth_ast__whileT       _while;       // while statements.
    cth_ast__switchT      _switch;      // switch statements.
    cth_ast__unaryT       _unary;       // unary ops.
    cth_ast__binaryT      _binary;      // binary ops.
    cth_ast__funcallT     _funcall;     // function call.
    cth_ast__programT     program;      // all of the program.
    cth_ast__declarationT _declaration; // variable declarations.
  } data;
};

// initialises abstract syntax tree normally.
// does not touch internals.
static cth_astT* cth_ast_init__weak(
  const cth_tokT op, const cth_fposT line, const cth_fposT offset,
  const cth_ast_typeT type
){
  cth_astT* const restrict node = calloc(1, sizeof(*node));

  node->op = op;
  node->line = line;
  node->offset = offset;
  node->type = type;

  return node;
}

// node->data initialised by _whoever receives it_, although some minor work done here.
// automatically sets *reference to node when not NULL.
static cth_astT* cth_ast_init(
  const cth_tokT op, const cth_fposT line, const cth_fposT offset,
  const cth_ast_typeT type
){
  cth_astT* const restrict node = cth_ast_init__weak(op, line, offset, type);

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
    case CTH_AST__FUNCALL:
      node->data._funcall.args = vfastarr_init(sizeof(*node->data._funcall.args));
      break;
    case CTH_AST__PROGRAM:
      node->data.program.program = vfastarr_init(sizeof(*node->data.program.program));
      break;
    case CTH_AST__DECLARATION:
      node->data._declaration.expr = vfastarr_init(sizeof(*node->data._declaration.expr));
      node->data._declaration.vars = vfastarr_init(sizeof(*node->data._declaration.vars));
      break;
    default: break;
  }

  return node;
}

static cth_astT* cth_ast_clone__recursive(cth_astT* const restrict src);

static cth_astT** cth_ast_clone__array(cth_astT** const restrict src){
  if(!src) return NULL;

  const size_t c_src = vfastarr_elcnt(src, sizeof(*src));
  cth_astT** const restrict dst = vfastarr_init_explicit(c_src, c_src, sizeof(*src));

  for(size_t i = 0; i < c_src; i++)
    dst[i] = cth_ast_clone__recursive(src[i]);

  return dst;
}

static cth_astT* cth_ast_clone__recursive(cth_astT* const restrict src){
  cth_astT* const restrict node = malloc(sizeof(*node));

  node->op = src->op;
  node->line = src->line;
  node->offset = src->offset;
  node->type = src->type;

  switch(src->type){
    case CTH_AST__ID:
      node->data.id = src->data.id;
      break;
    case CTH_AST__IF:
      node->data._if.condition = cth_ast_clone__recursive(src->data._if.condition);
      node->data._if.success = cth_ast_clone__array(src->data._if.success);
      node->data._if.failure = cth_ast_clone__array(src->data._if.failure);
      break;
    case CTH_AST__FOR:
      node->data._for.init = cth_ast_clone__recursive(src->data._for.init);
      node->data._for.condition = cth_ast_clone__recursive(src->data._for.condition);
      node->data._for.update = cth_ast_clone__recursive(src->data._for.update);
      node->data._for.code = cth_ast_clone__array(src->data._for.code);
      break;
    case CTH_AST__DEFER:
      node->data._defer.code = cth_ast_clone__array(src->data._defer.code);
      break;
    case CTH_AST__WHILE:
      node->data._while.condition = cth_ast_clone__recursive(src->data._while.condition);
      node->data._while.code = cth_ast_clone__array(src->data._while.code);
      break;
    case CTH_AST__SWITCH:
      node->data._switch.cases = cth_ast_clone__array(src->data._switch.cases);
      break;
    case CTH_AST__UNARY:
      node->data._unary.src = cth_ast_clone__recursive(src->data._unary.src);
      break;
    case CTH_AST__BINARY:
      node->data._binary.s0 = cth_ast_clone__recursive(src->data._binary.s0);
      node->data._binary.s1 = cth_ast_clone__recursive(src->data._binary.s1);
      break;
    case CTH_AST__FUNCALL:
      node->data._funcall.fun = cth_ast_clone__recursive(src->data._funcall.fun);
      node->data._funcall.args = cth_ast_clone__array(src->data._funcall.args);
      break;
    case CTH_AST__PROGRAM:
      node->data.program.program = cth_ast_clone__array(src->data.program.program);
      break;
    case CTH_AST__DECLARATION: 
      node->data._declaration.type = cth_ast_clone__recursive(src->data._declaration.type);
      node->data._declaration.expr = cth_ast_clone__array(src->data._declaration.expr);
      node->data._declaration.vars = vfastarr_clone(src->data._declaration.vars);
      break;
  }

  return node;
}

static cth_astT* cth_ast_clone(cth_astT* const restrict src){
  return cth_ast_clone__recursive(src);
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
      cth_ast_destroy__array(node->data._for.code);
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
    case CTH_AST__FUNCALL:
      cth_ast_destroy(node->data._funcall.fun);
      cth_ast_destroy__array(node->data._funcall.args);
      break;
    case CTH_AST__PROGRAM:
      cth_ast_destroy__array(node->data.program.program);
      break;
    case CTH_AST__DECLARATION:
      cth_ast_destroy(node->data._declaration.type);
      cth_ast_destroy__array(node->data._declaration.expr);
      vfastarr_destroy(node->data._declaration.vars);
      break;
  }
  free(node);
}

#endif
