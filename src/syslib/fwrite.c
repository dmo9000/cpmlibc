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



size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    int i = 0;
    ssize_t wr = 0;
    char *myptr = (char *) ptr;
    if (! stream || stream->_file == -1) {
        errno = EBADF;
        return -1;
    }

    for (i = 0; i < nmemb; i++) {
        wr = write(stream->_file, myptr, size);
        if (wr == 0) {
            /* END OF FILE REACHED */
            stream->_eof = true;
        }


        if (wr == -1) {
            /* pass through errno from write() */
            return 0;
        }
    }

    return i;
}

