# LIBRARIES ############################################################
ARLIB := lp.c enc.c
SOLIB := 

SOFLAGS := -shared -Wl,-rpath=$(LIBDIR)
ARFLAGS := -rcv

LIBS    := $(patsubst %.c,lib%.a,$(ARLIB)) \
           $(patsubst %.c,lib%.so,$(SOLIB))
