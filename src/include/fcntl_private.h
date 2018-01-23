extern FILE filehandles[FILES_MAX];
extern _cfd CFD[FILES_MAX];
extern bool _fds_init_done;
extern int  _find_free_fd();
extern int  _find_free_filehandle();
extern void _fds_init();
extern char * _print_fcb(FCB *fcb_ptr, bool brief);
extern uint8_t dma_buffer[SSIZE_MAX];
