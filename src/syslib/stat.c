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

int stat(char *pathname, struct stat *buf)
{

    static FCB statfcb;
    FCB *fcb_ptr = NULL;
    char _filename[9];
    char _filetype[4];
    int length = 0;
    int rval = 0;
    char *ptr = NULL;
    uint8_t current_extent = 0; /* 16K block index */
    uint8_t module_number = 0; /* 512K block index */
    uint16_t num_records = 0; /* number of 128 byte blocks */
    int i = 0;
    bool completed;

    if (!_fds_init_done) {
        _fds_init();
    }
    memset(&_filename, 0, 9);
    memset(&_filetype, 0, 4);
    ptr = strchr(pathname, '.');

    length = ptr - pathname;
    if (length > 8) {
        errno = ENOENT;
        return -1;
    }
    strncpy(_filename, pathname, length);
    length = strlen(ptr+1);
    if (length > 3) {
        errno = ENOENT;
        return -1;
    }
    strncpy(_filetype, ptr+1, ( length < 4 ? length : 3));

    completed = false;

    while (completed != true) {
        fcb_ptr = (FCB*) &statfcb;
        cpm_setFCBname(_filename, _filetype, (FCB*) fcb_ptr);
        fcb_ptr->ex = (current_extent % EXTENTS_PER_MODULE);
        //fcb_ptr->ex = (current_extent);
        module_number = (uint8_t) (current_extent / EXTENTS_PER_MODULE);
        //fcb_ptr->resv = (0x80 +  module_number) << 8;
        //fcb_ptr->resv =0x8080 +  module_number;
        if (module_number) {
            fcb_ptr->resv = 0x8081;
        } else {
            fcb_ptr->resv = 0x8080;
        }
        printf("offset = %lu\n", (uint32_t) ((num_records * 128)));
        printf("extent = %u (%u)\n", (current_extent % EXTENTS_PER_MODULE), current_extent);
        printf("module_number = %02x\n", module_number);
        printf("fcp_ptr->resv = 0x%04x\n", fcb_ptr->resv);
        rval = cpm_performFileOp(fop_open, fcb_ptr);
        printf("stat(%d, %d)\n", rval, fcb_ptr->rc);
        _print_fcb(fcb_ptr, false);

        if (module_number > 1) {
            printf("module number = %d (too large, limit 1M)\n", module_number);
            goto done_counting;
        }

        switch(rval) {
        case 0xFF:
            errno = ENOENT;
            return -1;
            break;
        case 0:
        case 1:
        case 2:
        case 3:
            /* examine the FCB in more detail */
            //_print_fcb(fcb_ptr, false);
            switch (fcb_ptr->rc) {
            case 0x00:
                /* no blocks in extent - we are finished */
                printf("fcb_ptr->rc = %d, rval = %d\n", fcb_ptr->rc, rval);
                printf("(empty extent?)\n");
                goto done_counting;
                break;
            default:
                printf("fcb_ptr->rc = %d, rval = %d\n", fcb_ptr->rc, rval);
                printf("%s", TTY_FOREGROUND_PURPLE);
                num_records += fcb_ptr->rc;
                printf("[multi-extent %u file:%u blocks (so far)]\n", current_extent, num_records);
                current_extent++;
                printf("%s", TTY_FOREGROUND_WHITE);
                break;

            }
            break;
        default:
            printf("unhandled, rval = %d\n", rval);
            exit(1);
            break;
        }
    }

done_counting:
    printf("[records=%u, extents=%u, size=%lu bytes]\n", num_records, current_extent, (uint32_t) num_records * 128);
    buf->st_mode = 0;
    buf->st_mode |= S_IFREG;
    buf->st_mode |= S_IREAD;
    buf->st_mode |= S_IWRITE;
    buf->st_atime = 0;
    buf->st_mtime = 0;

    //buf->st_size = ((num_records * 128) / 1024) + ((((num_records * 128) % 1024) ? 1 : 0)) * 1024;
    buf->st_size = (uint32_t) num_records * 128;
    errno = 0x0;
    return 0;

}




