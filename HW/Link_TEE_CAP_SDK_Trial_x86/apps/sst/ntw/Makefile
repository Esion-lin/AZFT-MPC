#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

TOP     := .

ifeq ($(CROSS_COMPILE_CC),)
CC     	:= $(CROSS_COMPILE_CC)gcc -m32
else
CC     	:= $(CROSS_COMPILE_CC)
endif

CFLAGS  += -Wall -g -Os -Wdeclaration-after-statement
CFLAGS  += -I$(TOP)/../inc
CFLAGS  += -I$(TOP)/../../../ntw/inc

CFLAGS  += -DCONFIG_TIME_TEST

LDFLAGS := -pthread


SRCS := $(wildcard *.c)
OBJS  := $(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(SRCS)))

LIBS  := $(TOP)/../../../out/libtee_client.so
LIBS  += $(TOP)/../../../out/libtee_internal.so

OUT   := ca_sst

$(OUT): $(OBJS)
	$(CC) $^ -o $@ -L$(TOP)/../../../out -ltee_client -ltee_internal -lpthread 

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $*.o

.PHONY: clean
clean:
	rm -rf $(OBJS) $(OUT)
