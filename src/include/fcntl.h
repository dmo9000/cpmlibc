#ifndef __FNCTL_H__
#define __FNCTL_H__

#include <cpm_sysfunc.h>
#include <limits.h>

#define O_ACCMODE    0003
#define O_RDONLY       00
#define O_WRONLY       01
#define O_RDWR         02
#define O_TRUNC     01000

#define O_CREAT		00000100	/* not fcntl */
#define O_EXCL		00000200	/* not fcntl */

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

#define FILES_BASE                  0x03
#define FILES_MAX                   4 
#define MODULE_SIZE                 524288
#define EXTENTS_PER_MODULE          32
#define KILOBYTE                    1024
#define EXTENT_SIZE                 (16 * KILOBYTE)


typedef struct {
    int id;
    int oflags;
    /* FIXME: should be off_t ? */
    uint32_t offset;
    FCB fcb;
} _cfd;

#endif /* __FCNTL_H__ */
