LIBC_DIR = libc

# Here begins the actual creation of destination files

CRT_OBJS = $(LIBC_DIR)/cpm0.rel
CPM_OBJS = $(LIBC_DIR)/cpmbdos.rel
HW_OBJS	 = $(LIBC_DIR)/hw_common.rel
OBJS =		 $(LIBC_DIR)/vprintf.rel $(LIBC_DIR)/cprintf.rel $(LIBC_DIR)/cstdio.rel $(LIBC_DIR)/ansi_term.rel $(LIBC_DIR)/cpm_sysfunc.rel 		\
           $(LIBC_DIR)/strtol.rel $(LIBC_DIR)/fcntl.rel $(LIBC_DIR)/errno.rel $(LIBC_DIR)/string.rel $(LIBC_DIR)/ctype.rel $(LIBC_DIR)/random.rel									 	\
           $(LIBC_DIR)/getopt.rel $(LIBC_DIR)/lldetect.rel $(LIBC_DIR)/llclock.rel $(LIBC_DIR)/llcommand.rel 	\
					 $(LIBC_DIR)/heap.rel $(LIBC_DIR)/malloc.rel $(LIBC_DIR)/time.rel $(LIBC_DIR)/tcp.rel $(LIBC_DIR)/grx.rel $(LIBC_DIR)/llgrx.rel $(LIBC_DIR)/llnet.rel


libraries: libc/libc.a

libc/libc.a:$ $(CPM_OBJS) $(HW_OBJS) $(OBJS)
	sdcc-sdar -rc $(LIBC_DIR)/libc.a $(OBJS)

$(LIBC_DIR)/llcommand.rel: src/syslib/llcommand.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/llcommand.rel src/syslib/llcommand.s

$(LIBC_DIR)/lldetect.rel: src/syslib/lldetect.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/lldetect.rel src/syslib/lldetect.s

$(LIBC_DIR)/llclock.rel: src/syslib/llclock.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/llclock.rel src/syslib/llclock.s

$(LIBC_DIR)/llgrx.rel: src/syslib/llgrx.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/llgrx.rel src/syslib/llgrx.s

$(LIBC_DIR)/llnet.rel: src/syslib/llnet.s
	$(CAS) $(CAS_FLAGS) $(LIBC_DIR)/llnet.rel src/syslib/llnet.s

libraries-install: $(LIBC_DIR)/cpm0.rel $(LIBC_DIR)/libc.a
	sudo rm -rf /usr/share/sdcc/lib/z80cpm
	sudo rm -rf /usr/share/sdcc/lib/z80cpm
	sudo mkdir -p /usr/share/sdcc/lib/z80cpm
	sudo cp $(LIBC_DIR)/cpm0.rel $(LIBC_DIR)/libc.a /usr/share/sdcc/lib/z80cpm
	sudo mkdir -p /usr/share/sdcc/z80cpm/include
	sudo cp -rfp src/include/* /usr/share/sdcc/z80cpm/include

libraries-clean:
	rm -f $(LIBC_DIR)/*.rel $(LIBC_DIR)/*.asm $(LIBC_DIR)/*.lst $(LIBC_DIR)/*.map $(LIBC_DIR)/*.noi $(LIBC_DIR)/*.sym $(LIBC_DIR)/*.a

$(LIBC_DIR)/%.rel: $(SYSLIB_SRC_DIR)/%.c
	$(CCC) $(CCC_FLAGS) -o $@ -c $<

$(LIBC_DIR)/cpmbdos.rel:	$(SRC_DIR)/cpm/cpmbdos.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR)/cpmbdos.rel $(SRC_DIR)/cpm/cpmbdos.c

$(LIBC_DIR)/hw_common.rel: $(HWLIB_SRC_DIR)/common/hw_common.c
	$(CCC) $(CCC_FLAGS) -o $(LIBC_DIR)/hw_common.rel $(HWLIB_SRC_DIR)/common/hw_common.c

# Build CP/M-80 Command File Structure files
$(LIBC_DIR)/cpm0.rel: $(CPM_SRC_DIR)/cpm0.s
	$(CAS) $(CAS_FLAGS) $(CPM_SRC_DIR)/cpm0.rel $(CPM_SRC_DIR)/cpm0.s
	$(QUIET)$(COPY) $(CPM_SRC_DIR)/cpm0.rel $(LIBC_DIR)
	$(QUIET)$(COPY) $(CPM_SRC_DIR)/cpm0.lst $(LIBC_DIR)
