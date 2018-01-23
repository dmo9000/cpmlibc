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

int close(int fd)
{

    FCB *fcb_ptr = NULL;
    int rval = 0 ;
    if (!_fds_init_done) {
        _fds_init();
    }

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }
    /* errno should be set appropriately */
    if (CFD[fd].id != -1) {
        fcb_ptr = &CFD[fd].fcb;
//        printf("%s [closing FCB at %d:0x%04x (%d)]\n", _print_fcb(fcb_ptr, true), fd, fcb_ptr, sizeof(FCB), _print_fcb(fcb_ptr, true));
        //_print_fcb(fcb_ptr, true);
        rval = cpm_performFileOp(fop_close, fcb_ptr);
        //printf(" close->ret.val %02X\n", rval);
        //fds[fd] = -1;
        CFD[fd].id = -1;
        errno = 0;
        return 0;
    }
    errno = EBADF;
    return -1;
}





