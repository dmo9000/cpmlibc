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

/* errno should be set appropriately */

off_t lseek(int fd, off_t offset, int whence)
{

    if (!_fds_init_done) {
        _fds_init();
    }

    /* here we just verify offsets and tweak the pointer around. the real magic happens in read/write */

    //printf("\n+lseek(%d, %u, %d)\n", fd, offset, whence);

    if (CFD[fd].id == -1) {
        errno = EBADF;
        return -1;
    }
    if (whence == SEEK_SET) {
        CFD[fd].offset = (uint32_t) offset;
        errno = 0;
        return 0;
    }


    errno = EINVAL;
    return -1;
}



