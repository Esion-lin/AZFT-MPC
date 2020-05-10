#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

TOP     := .
CC      := g++ -m32
CFLAGS  += -Wall -g -Os
CFLAGS  += -I$(TOP)/../inc
CFLAGS  += -I$(TOP)/../../../ntw/inc

CFLAGS  += -DCONFIG_TIME_TEST

SRCS := $(wildcard *.cpp)
OBJS  := $(patsubst %.cxx,%.o,$(patsubst %.cpp,%.o,$(SRCS)))

LIBS  := $(TOP)/../../../out/libtee_client.so
LIBS  += $(TOP)/../../../out/libtee_internal.so

OUT   := ca_xor

$(OUT): $(OBJS)
	$(CC) $^ -o $@ -L$(TOP)/../../../out -ltee_client -ltee_internal -lpthread 

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $*.o

.PHONY: clean
clean:
	rm -rf $(OBJS) $(OUT)
