# Copyright (c) 2003-2004 E. Will et al.
# Rights to this code are documented in LICENSE.
#
# This file contains build instructions.
#

MODULE = contrib/adminserv

SRCS = $(wildcard *.c)

include ../../../extra.mk
include ../../../buildsys.mk
include ../../../buildsys.module.mk

CPPFLAGS	+= -I../../../include
CFLAGS		+= ${PLUGIN_CFLAGS}
LIBS		+=	-L../../../libathemecore -lathemecore ${LDFLAGS_RPATH}

