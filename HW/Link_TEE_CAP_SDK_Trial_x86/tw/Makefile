#
# Copyright (C) 2015-2017 Alibaba Group Holding Limited
#

#########################################################################
# COMMON COMPILATION FLAGS                                              #
#########################################################################


CC 		:= gcc -m32
LD      := $(CROSS_COMPILE)ld
CPP	:= $(CC) -E
AR	:= $(CROSS_COMPILE)ar
NM	:= $(CROSS_COMPILE)nm
LDR	:= $(CROSS_COMPILE)ldr
STRIP	:= $(CROSS_COMPILE)strip
OBJCOPY	:= $(CROSS_COMPILE)objcopy
OBJDUMP	:= $(CROSS_COMPILE)objdump
RM      := rm -rf

CFLAGS  += -Wall -Os
CFLAGS  += -Wdeclaration-after-statement

DEBUG   ?= 0

ifeq ($(DEBUG), 1)
CFLAGS  += -DDEBUG -O0 -g
endif



SUBDIRS := .
OUT_DIR := ../out
CFLAGS  += -I./inc
CFLAGS  += -I../apps/sst/inc


VPREFIX		?= @

MAJOR_VERSION	:= 1
MINOR_VERSION	:= 0

LIB_NAME        := libtee_internal
LIB_INT         := $(LIB_NAME).so
LIB_S_INT       := $(LIB_NAME).a
TEEI_S_LIBRARY	:= ./lib/$(LIB_S_INT)
LIB_INT_MAJ_MIN	:= $(LIB_INT).$(MAJOR_VERSION).$(MINOR_VERSION)
TEEI_LIBRARY	:= $(OUT_DIR)/$(LIB_INT_MAJ_MIN)

SRCS := ./src/tee_srv_table.c \
        ../apps/xor/tw/ta_xor.c
        
all: tee_a

OBJS := $(patsubst %.c, %.o, $(SRCS))

tee_a: $(TEEI_LIBRARY)
	$(VPREFIX)ln -sf $(LIB_INT_MAJ_MIN) $(OUT_DIR)/$(LIB_INT)
	

$(TEEI_LIBRARY): $(OBJS)
	@mkdir -p $(OUT_DIR)
	@echo "  LD      $@ "
	$(VPREFIX)$(CC) $(CFLAGS) -shared -o $@ $^ -ldl $(TEEI_S_LIBRARY)

%.o: %.c
	$(VPREFIX)$(CC) -c $(CFLAGS) -fPIC $< -o $@

clean:
	rm -rf $(OBJS) $(TEEI_LIBRARY) $(OUT_DIR)/$(LIB_INT)
