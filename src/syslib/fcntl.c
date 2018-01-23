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
uint8_t dma_buffer[SSIZE_MAX];
FILE filehandles[FILES_MAX];
_cfd CFD[FILES_MAX];
bool _fds_init_done = false;
int  _find_free_fd();
int  _find_free_filehandle();
void _fds_init();

void _fds_init()
{
    int i = 0;
//    printf("_fds_init()\n");
    for (i = 0; i < FILES_MAX; i++) {
        //fds[i] = -1;
        memset(&CFD[i], 0, sizeof(_cfd));
        CFD[i].id = -1;
        /* setup filehandles */
        filehandles[i]._file = -1;
    }
    _fds_init_done = true;
    return;
}

char * _print_fcb(FCB *fcb_ptr, bool brief)
{
    int i = 0;
    static char fbuffer[64];
    char *p = (char *) &fbuffer;
    memset(&fbuffer, 0, 64);
    printf("%s", TTY_FOREGROUND_RED);
    if (brief) {
        sprintf((const char *) &fbuffer, "[%c%c%c%c%c%c%c%c.%c%c%c]", fcb_ptr->filename[0], fcb_ptr->filename[1], fcb_ptr->filename[2], fcb_ptr->filename[3], fcb_ptr->filename[4],
                fcb_ptr->filename[5], fcb_ptr->filename[6], fcb_ptr->filename[7],
                fcb_ptr->filetype[0], fcb_ptr->filetype[1], fcb_ptr->filetype[2]);
        printf("%s", TTY_FOREGROUND_WHITE);
        return (char*) p;
    }

    printf("\n");
    printf("\tdrive ->\t%u\n", fcb_ptr->drive);
    printf("\tname  ->\t%c%c%c%c%c%c%c%c\n", fcb_ptr->filename[0], fcb_ptr->filename[1], fcb_ptr->filename[2], fcb_ptr->filename[3], fcb_ptr->filename[4], fcb_ptr->filename[5], fcb_ptr->filename[6], fcb_ptr->filename[7]);
    printf("\ttype  ->\t%c%c%c\n", fcb_ptr->filetype[0], fcb_ptr->filetype[1], fcb_ptr->filetype[2]);
    printf("\t  ex  ->\t%02X\n",fcb_ptr->ex);
    printf("\tresv  ->\t%04X\n",fcb_ptr->resv);
    printf("\t  rc  ->\t%02X\n",fcb_ptr->rc);
    printf("\talb0  ->\t%04X %04X %04X %04X\n", fcb_ptr->alb[0], fcb_ptr->alb[1], fcb_ptr->alb[2], fcb_ptr->alb[3]);
    printf("\talb1  ->\t%04X %04X %04X %04X\n", fcb_ptr->alb[4], fcb_ptr->alb[5], fcb_ptr->alb[6], fcb_ptr->alb[7]);
    printf("\tsreq  ->\t%02X\n",fcb_ptr->seqreq);
    printf("\trrec  ->\t%04X\n",fcb_ptr->rrec);
    printf("\trreo  ->\t%02X\n\n",fcb_ptr->rrecob);

    printf("%s", TTY_FOREGROUND_WHITE);
    return (char*) p;
}

int  _find_free_fd()
{
    int i = 0;
    if (!_fds_init_done) {
        _fds_init();
    }
    for (i = FILES_BASE; i < FILES_MAX; i++) {
        if (CFD[i].id == -1) {
            return i;
        }
    }
    return -1;
}

int  _find_free_filehandle()
{
    int i = 0;
    if (!_fds_init_done) {
        _fds_init();
    }

    for (i = 0; i < FILES_MAX; i++) {
        if (filehandles[i]._file == -1) {
            return i;
        }
    }
    return -1;
}

