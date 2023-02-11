#define _GNU_SOURCE
#include <linux/memfd.h>
#include <linux/unistd.h>
#include <dlfcn.h>
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "libfuse2inc.h"

#define ENABLE_DLOPEN
#include "squashfuse_dlopen.h"

void *libhandle = NULL;
int have_libloaded = 0;

const char *load_library_errmsg =
  "AppImages require FUSE to run. \n"
  "You might still be able to extract the contents of this AppImage \n"
  "if you run it with the --appimage-extract option. \n"
  "See https://github.com/AppImage/AppImageKit/wiki/FUSE \n"
  "for more information\n";

static void *dlopen_builtinfuse(void)
{
  unsigned long libfuse_so_2_len = 2000 * 1024;
  unsigned char *libfuse_so_2 = malloc(libfuse_so_2_len);
  int ret = uncompress(libfuse_so_2, &libfuse_so_2_len, libfuse_so_2_zlib, libfuse_so_2_zlib_len);
  if (ret == Z_OK) {
    int fd = syscall(__NR_memfd_create, "shm_anon", (unsigned int)(MFD_CLOEXEC));
    if (fd < 0) {
      free(libfuse_so_2);
      return NULL;
    }

    ret = write(fd, libfuse_so_2, libfuse_so_2_len);
    free(libfuse_so_2);
    if (ret != libfuse_so_2_len) {
      return NULL;
    }

    char path[256];
    snprintf(path, 256, "/proc/%d/fd/%d", getpid(), fd);
    return dlopen(path, RTLD_LAZY);
  } else {
    printf("err %d %d %d %d\n", ret, Z_BUF_ERROR, Z_MEM_ERROR, Z_DATA_ERROR);
  }
}

void load_library(void)
{
  if (have_libloaded != 1) {
    libhandle = dlopen(LIBNAME, RTLD_LAZY);
    if (!libhandle)
      libhandle = dlopen_builtinfuse();
    if (!libhandle) {
      fprintf(stderr, "dlopen(): error loading " LIBNAME "\n\n%s", load_library_errmsg );
      exit(1);
    } else {
      have_libloaded = 1;
    }
  }
}
