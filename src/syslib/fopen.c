#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl_private.h>
#include "cpmbdos.h"
#include "cpm_sysfunc.h"
#include "ansi_term.h"
#include "tty.h"

/*
		If you are writing an emulator at BDOS level, you need to be aware of how CP/M uses the bytes EX, S2, and CR. Some programs (such as the Digital Research linker,
		LINK.COM) manipulate these bytes to perform "seek" operations in files without using the random-access calls.

			CR = current record,   ie (file pointer % 16384)  / 128
			EX = current extent,   ie (file pointer % 524288) / 16384
			S2 = extent high byte, ie (file pointer / 524288). The CP/M Plus source code refers to this use of the S2 byte as 'module number'.
*/



FILE *fopen(const char *path, const char *mode)
{
    static struct stat statbuf;
    int fd = -1;
    int fh = -1;
    int oflags = 0;
    FILE *myfhptr = NULL;
    ssize_t initial_size = 0;
    if (!_fds_init_done) {
        _fds_init();
    }

    if (strncmp(mode, "r", 1) == 0) {
        oflags = O_RDONLY;
    }

    if (strncmp(mode, "w", 1) == 0) {
        oflags = O_WRONLY | O_TRUNC;
    }

    if (strncmp(mode, "r+", 2) == 0) {
        oflags = O_RDWR;
    }

    if (strncmp(mode, "w+", 2) == 0) {
        oflags = O_RDWR | O_TRUNC;
    }
    errno = 0;
    fd = open(path, oflags);

    if (fd < 0) {
        /* pass through errno */
        return NULL;
    }
    errno = 0;
    /* get free filehandle */
    fh = _find_free_filehandle();
    if (fh == -1) {
        close(fd);
        errno = ENFILE;
        return NULL;
    }
    errno = 0;
    filehandles[fh]._file = fd;
    filehandles[fh]._eof = false;
    filehandles[fh]._limit = initial_size;
    memset(&filehandles[fh]._flags, 0, 4);
    strncpy((const char *) &filehandles[fh]._flags, (const char *) mode, 3);
    myfhptr = &filehandles[fh];
    return (FILE*) myfhptr;

}

