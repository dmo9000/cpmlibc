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


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    ssize_t ritems = 0;
    int i = 0;
    size_t bytes = 0;
    int rd = 0;
    char *myptr = (char*) ptr;
    char *eofptr = NULL;
    if (! stream || stream->_file == -1) {
        errno = EBADF;
        return -1;
    }

    for (i = 0; i < nmemb; i++) {
        rd = read(stream->_file, myptr, size);
        if (rd == 0) {
            /* END OF FILE REACHED */
            stream->_eof = true;
            //printf("HARD EOF REACHED\n");
            //exit(1);
            /*
            if (ftell(stream) != stream->_limit) {
            printf("HARD EOF REACHED, BUT LIMIT IS MISMATCHED\n");
            }
            */
        }

        if (rd == -1) {
            printf("fread() error; errno = %d [%s]\n", errno, strerror(errno));
            /* fread() returns 0 on error (or a short item count) and leaves it to the caller to determine what happened */
            return(0);
        }

        if (rd == size) {
            myptr += rd;
            ritems++;
        } else {
            if (rd < size) {
                errno = EIO;
                return ritems;
            }
            errno = EIO;
            return ritems;
        }
    }

    /* before returning, check the 'b' flag on the FILE. if it is not set, we need to search for the EOF marker in this block */


    if (memchr(&stream->_flags, 'b', 3) == NULL) {
        /* if the file was not opened in binary mode, we should respect that character 0x1A is the EOF marker */
        eofptr = memchr((const char *) ptr, 0x1A, size * nmemb);
        if (eofptr) {
            stream->_limit = CFD[stream->_file].offset + (eofptr - ptr);
            stream->_eof = true;
            //CFD[stream->_file].offset = stream->_limit;
            //printf("current offset = %lu\n", CFD[stream->_file].offset);
            //`printf("rewind = %d\n", (SSIZE_MAX - (eofptr - ptr)));
            CFD[stream->_file].offset -= (SSIZE_MAX - (eofptr - ptr));
            stream->_limit = CFD[stream->_file].offset;
        } else {
        }
    }


    errno = 0;
    return ritems;
}

