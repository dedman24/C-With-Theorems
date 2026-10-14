#ifndef SATISFIABLE_COMMON_FILEOPS_H_INCLUDED
#define SATISFIABLE_COMMON_FILEOPS_H_INCLUDED

// stdlib includes.
#include "stdio.h"        // FILE & file ops.

// POSIX includes.
#include "fcntl.h"        // open.
#include "unistd.h"       // close.
#include "sys/mman.h"     // mmap.
#include "sys/stat.h"     // fstat.

typedef struct{
  char* code;
  size_t size;
} satisfiable__fileT;

#define SATISFIABLE__FILE_NULL ((satisfiable__fileT){ NULL, 0 })

static satisfiable__fileT satisfiable_load__map(const char* const restrict path, const bool suppresserrors){
// use fstat to get file size.
// load file using mmap.
  const int fd = open(path, O_RDONLY);
  if(fd < 0){
    if(!suppresserrors)
      fprintf(stderr, "ERROR: could not open file '%s'.\n", path);
    return SATISFIABLE__FILE_NULL;
  }

  struct stat st;
  if(fstat(fd, &st) < 0){
    close(fd);
    if(!suppresserrors)
      fprintf(stderr, "ERROR: could not fstat file '%s'.\n", path);
    return SATISFIABLE__FILE_NULL;
  }

  char* const restrict m_file = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if(!m_file){
    close(fd);
    if(!suppresserrors)
      fprintf(stderr, "ERROR: could not mmap file '%s'.\n", path);
    return SATISFIABLE__FILE_NULL;
  }
  close(fd);

  return (satisfiable__fileT){ .code = m_file, .size = st.st_size };
}

static void satisfiable_load__unmap(const satisfiable__fileT file){
  munmap(file.code, file.size);
}

#endif
