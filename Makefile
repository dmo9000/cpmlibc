MAXALLOCS=200000
#MAXALLOCS=5000
#MAXALLOCS=1
# cat /proc/cpuinfo | grep "^processor" | wc -l
#MAXALLOCS=100000

include common.mk

CFLAGS = --std-c99 --fverbose-asm
CLD_FLAGS=-k /usr/share/sdcc/lib/z80/
MIRRORS=/media/sf_OSZ/systems/199c4422-9790-11e7-be3e-0800278237b3 /media/sf_OSZ/systems/199c4422-9790-11e7-be3e-0800278237b3 /media/sf_OSZ/systems/199c4422-9790-11e7-be3e-0800278237b3
IMGDIRS := $(shell ls -1  /media/sf_OSZ/systems/ 2>/dev/null )
CPUCORES := $(shell nproc)
THISBUILDID := $(shell cat build-id )

all: 	bump-build-number libraries 

bump-build-number: 
	./build-id.sh	
	@echo "+++ build-id for this build is ${THISBUILDID}"
	@echo "+++ Using ${CPUCORES} available CPU cores"


clean-arf:
	find . -name "*.arf" -print -exec rm -f {} \;

clean:	libraries-clean

	find . -name "*.rel" ! -name "isr0.rel" -exec rm -f {} \;
	find . -name "*.sym" -exec rm -f {} \;
	find . -name "*.lst" -exec rm -f {} \;
	find . -name "*.asm" -exec rm -f {} \;


install: libraries-install

include libraries.mk # Libraries
