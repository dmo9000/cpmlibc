# Optimization
#MAXALLOCS=100000

# Prefixes
COMPILER_PREFIX = $(SDCC_PREFIX)
COMPILER_LIBS = /usr/share/sdcc/lib/z80/

# Options
QUIET = @

# SDCC commands
CCC = $(COMPILER_PREFIX)/bin/sdcc
CAS = $(COMPILER_PREFIX)/bin/sdasz80
CLD = $(COMPILER_PREFIX)/bin/sdldz80

# Local CC
CC = gcc

# Misc local commands
ECHO = echo
COPY = cp
MOVE = mv
SED = sed

# Project directories
SRC_DIR = src
CPM_SRC_DIR = $(SRC_DIR)/cpm
SYSLIB_SRC_DIR = $(SRC_DIR)/syslib
HWLIB_SRC_DIR = $(SRC_DIR)/hw
BIN_DIR = bin

LSRC_DIR = lsrc
LBIN_DIR = lbin
ESRC_DIR = esrc

INCLUDE_DIR = -I$(SRC_DIR)/include

# Compilation / Assembly / Linking flags
CCC_FLAGS = -D__CPM__ -D__SDCC__=1 --std-c99 --fverbose-asm -c -mz80 --opt-code-size --max-allocs-per-node $(MAXALLOCS) $(INCLUDE_DIR)


CAS_FLAGS = -plosff
CLD_FLAGS =

