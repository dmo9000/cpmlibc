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

ssize_t read(int fd, void *buf, size_t count)
{
    FCB *fcb_ptr = NULL;
    uint16_t ret_ba, ret_hl;
    int rval = 0;
    int cpm_err = 0;
    int rd = 0;
    int start_offset = 0;
    int limit = SSIZE_MAX;
    uint16_t required_module = 0;
    uint16_t required_extent = 0;
    uint16_t required_block = 0;
    if (!_fds_init_done) {
        _fds_init();
    }
    //printf("read(%d, 0x%04x, %u)\n", fd, buf, count);

    if ((fd < 0 || fd >= FILES_MAX) || CFD[fd].id == -1) {
        errno = EBADF;
        return -1;
    }

    if (count > SSIZE_MAX) {
        /* apparently behaviour is supposed to be undefined here, but we will return I/O error */
        errno = EIO;
        return -1;
    }
    fcb_ptr = (FCB*) &CFD[fd].fcb;

    if (CFD[fd].offset % SSIZE_MAX) {
        start_offset = CFD[fd].offset % SSIZE_MAX;
        limit -= start_offset;
    }

    required_module = (CFD[fd].offset / 524288);
    required_extent = (CFD[fd].offset / 16384);
    required_block = (CFD[fd].offset / 128);
    required_block -= (required_extent * 0x80);

    if (fcb_ptr->ex != required_extent) {
        fcb_ptr->ex = required_extent;
        rval = cpm_performFileOp(fop_open, fcb_ptr);
    }

    /*
       if (fcb_ptr->ex >= 512) {
            printf("whoah silver! how did we end up in this situation? fcb_ptr->ex = %d\n", fcb_ptr->ex);
            printf("That is pretty big for a CP/M file.\n");
            exit(1);
            }
    */

    if (fcb_ptr->seqreq != required_block) {
        fcb_ptr->seqreq = required_block;
    }

    fcb_ptr->rrec = 0x0000;
    fcb_ptr->rrecob = 0x00;
    fcb_ptr->resv = 0x8000 + (0x80 + required_module);
    cpm_setDMAAddr((uint16_t)dma_buffer);
    rval = cpm_performFileOp(fop_readSeqRecord, fcb_ptr);
    ret_ba = get_ret_ba();
    ret_hl = get_ret_hl();

//#define DEBUG_LIBCIO_READ
#ifdef DEBUG_LIBCIO_READ
    printf("%s", TTY_FOREGROUND_RED);
    printf(" read ret.val=%02X, ret_ba=0x%04x ret_hl=0x%04x\n", rval, ret_ba, ret_hl);
    printf("\ts1s2 ->\t%04X\n",fcb_ptr->resv);
    printf("\tex   ->\t%02X\n",fcb_ptr->ex);
    printf("\trc   ->\t%02X\n",fcb_ptr->rc);
    printf("\tsreq ->\t%02X\n",fcb_ptr->seqreq);
    printf("%s", TTY_FOREGROUND_WHITE);
#endif /* DEBUG_LIBCIO_READ */

    if (rval != 0) {
        cpm_err = (ret_ba & 0xff00) >> 8;
        if (fcb_ptr->rrec > 0x00ff) {
            printf("\n# READ_FAIL: ret.val=%02X, ret_ba=0x%04x ret_hl=0x%04x, CPM_ERR=%d\n", rval, ret_ba, ret_hl, cpm_err);
            printf("#\tsreq ->\t%02X\n",fcb_ptr->seqreq);
            printf("#\trrec ->\t%04X\n",fcb_ptr->rrec);
            printf("#\trreo ->\t%02X\n\n",fcb_ptr->rrecob);
        }

        switch(cpm_err) {
        case 0x01:
            /* end of file - return 0 to caller, clear errno */
            printf("/* read() hit EOF */\n");
            errno = 0;
            return 0;
            break;
        }

        /* something bad happened? */
        errno = EIO;
        return -1;
    }

    /* update RR record */

    rval = cpm_performFileOp(fop_updRandRecPtr, fcb_ptr);

    /* if we requested more bytes than are available, just copy those and return the value */
    if (count < limit) {
        limit = count;
    }

    /* copy to target buffer - TODO - see if we can work zero copy in here */
    memcpy(buf, &dma_buffer + start_offset, limit);
    /* TODO: return the number of bytes actually read */

    CFD[fd].offset += limit;

    errno = 0;

    return limit;
}




