#ifndef VFASTPATH_H_INCLUDED
#define VFASTPATH_H_INCLUDED

// vfastpath v1.0.
// vfastpath is a simple cc0 library that helps build fully qualified paths.
//
// algorithm (reduce):
//  if path to reduce doesn't start with '/', error out.
//  have stack of elements & stack of sizes.
//  push to stack of elements everything separated by slashes.
//  if element is '..', instead of pushing, pop one element.
//  final path is
// woah all C code requires so many stdlib includes!
// stdlib includes.
#include "stddef.h"       // for size_t, NULL.
#include "stdlib.h"       // for malloc.
#include "string.h"       // for string ops.
#include "stdbool.h"      // for bool type.

// functions.
// given path, produces new path equal in function to original one but reduced.
char* vfastpath_reduce(char* const restrict path, const size_t s_path);
// given two paths, appends them & reduces result.
char* vfastpath_append(const char* const restrict base, const size_t s_base, const char* const restrict suffix, const size_t s_suffix);

# ifdef VFASTPATH_IMPLEMENTATION

#define VFASTARR_IMPLEMENTATION
#include "vfastarr.h"

// TODO: add windows path support some day.
#  ifdef VFASTPATH_WINDOWS

#  else

// reduces path.
// path ~ NULL-terminated C string to reduce.
// s_path ~ path length without including string NULL terminator.
char* vfastpath_reduce(char* restrict path, const size_t s_path){
  if(!s_path) return NULL;
  const bool isroot = path[0] == '/';

  char**  restrict dirs   = vfastarr_init(sizeof(*dirs));
  size_t* restrict s_dirs = vfastarr_init(sizeof(*s_dirs));
// parses path
  while(path[0]){
    const size_t s_elem = strcspn(path, "/");
    if(s_elem == 0){ path++; continue; }                                //
    if(s_elem == 2 && path[0] == '.' && path[1] == '.'){                // '..' case.
      vfastarr_remove(dirs, sizeof(*dirs));
      vfastarr_remove(s_dirs, sizeof(*s_dirs));
    }
    else if(s_elem != 1 || path[0] != '.'){                             // '.' case.
      dirs = vfastarr_append(dirs, (void*)&path, sizeof(path));
      s_dirs = vfastarr_append(s_dirs, (void*)&s_elem, sizeof(s_elem));
    }
    path += s_elem + 1;
  }

  char* restrict fq_path = malloc(isroot + 1);                          // initialises to root dir.
  if(isroot) fq_path[0] = '/';

  size_t s_fq_path = isroot;
  const size_t c_dirs = vfastarr_elcnt(dirs, sizeof(*dirs));
  for(size_t i = 0; i < c_dirs; i++){
    const size_t isdir = (i == c_dirs - 1? path[-1] == '/': 1);         // checks whether last entry is file or directory.
    fq_path = realloc(fq_path, s_fq_path + s_dirs[i] + isdir + 1 /* this +1 is for the final NULL terminator */);
    memcpy(fq_path + s_fq_path, dirs[i], s_dirs[i]);

    if(isdir) fq_path[s_fq_path + s_dirs[i]] = '/';                     // adds dir marker.

    s_fq_path += s_dirs[i] + isdir;
  }

  fq_path[s_fq_path] = '\0';                                            // null-terminates strings.
  return fq_path;
}

char* vfastpath_append(const char* const restrict base, const size_t s_base, const char* const restrict suffix, const size_t s_suffix){
  char* restrict path = malloc(s_base + s_suffix + 1);
  memcpy(path, base, s_base);
  memcpy(path + s_base, suffix, s_suffix);
  path[s_base + s_suffix] = '\0';

  char* const restrict fq_path = vfastpath_reduce(path, s_base + s_suffix);
  free(path);
  return fq_path;
}

#  endif
# endif
#endif
