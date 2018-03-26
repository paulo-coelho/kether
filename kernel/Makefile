obj-m += kecho.o krec.o kclient.o

COMMON_FILES:= eth.o stats.o

kecho-y:= \
	$(COMMON_FILES) \
	echo.o

kclient-y:= \
	$(COMMON_FILES) \
	mclient.o

EXTRA_CFLAGS := -Wall -I$(PWD)/include -O2 # $(ADDITIONAL_FLAG)
ccflags-y := -std=gnu99 -Wall -Wno-declaration-after-statement

all:
	-mkdir build
	-touch build/Makefile
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/build src=$(PWD) modules


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	-rm -rf build





##SRC_DIR = $(PWD)
##OBJ_DIR = $(PWD)/build
##SRC = $(wildcard *.c)
##OBJ = $(SRC:.c=.o)
##KDIR ?= /lib/modules/$(shell uname -r)/build
##OBJ_DIR_MAKEFILE = $(OBJ_DIR)/Makefile
##
##EXTRA_CFLAGS = -I$(PWD)/include -std=gnu99 -Wall  -O3
##CFLAGS_MODULE=-Werror
##
##obj-m += kecho1.o 
##kecho-objs := eth.o kecho.o#$(OBJ)
##
##
##.PHONY: all clean 
##
##
##
##all:
##	-mkdir $(OBJ_DIR)
##	-touch $(OBJ_DIR_MAKEFILE)
##	make -C $(KDIR) M=$(OBJ_DIR) src=$(SRC_DIR) modules
##
##
##
##
#############################################################################
##clean:
##	make -C $(KDIR) M=$(OBJ_DIR) src=$(SRC_DIR) modules
##	-rm -rf $(OBJ_DIR)
##
##