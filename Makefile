#/***********************************************/#
#/**  MAC 0323  -  Estrutura de Dados          **/#
#/**  IME-USP   -  Primeiro  Semestre de 2013  **/#
#/**  Turma 45  -  Yoshiharu Kohayakawa        **/#
#/**                                           **/#
#/**  Terceiro  Exercício-Programa             **/#
#/**  Arquivo:  Makefile                       **/#
#/**                                           **/#
#/**  Renato Cordeiro Ferreira        7990933  **/#
#/***********************************************/# 

## PROGRAMS ###########################################################
CC := gcc
RM := rm -f
CP := cp -f
SED := sed -r
FMT := fmt -1
CAT := cat
MAKE += --no-print-directory
MKDIR := mkdir -p
RMDIR := rmdir --ignore-fail-on-non-empty

# DIRECTORIES ##########################################################
SRCDIR := .
OBJDIR := .
BINDIR := .
LIBDIR := .
DOCDIR := .
CONFDIR := .
TESTDIR := .
HEADDIR := .

-include directories.mk
VPATH = $(SRCDIR):$(LIBDIR):$(BINDIR):$(TESTDIR):$(HEADDIR)

## FILES ##############################################################
BIN := ep3
SRC := $(shell ls $(SRCDIR)/*.c)
LIB := $(CONFDIR)/libraries.mk
DEP := $(addprefix $(CONFDIR)/,$(SRC:.c=.d))

-include $(LIB)
OBJ := $(filter-out $(ARLIB) $(SOLIB),$(SRC)) # Tira bibliotecas
OBJ := $(patsubst %.c,%.o,$(OBJ))             # Substitui .c por .o
OBJ := $(addprefix $(OBJDIR)/,$(OBJ))         # Adiciona diretório

## COMPILATION ########################################################
CLIBS  := -I. $(patsubst %,-I%,$(filter-out .%,$(shell find $(HEADDIR) -type d)))
CFLAGS := -ansi -Wall -pedantic -g 

## LINKAGE ############################################################
LDLIBS  := -L. $(patsubst %,-I%,$(filter-out .%,$(shell find $(LIBDIR) -type d)))
LDFLAGS := -lm
LDFLAGS += -Wl,-rpath,$(LIBDIR)
LDFLAGS += $(filter -l%,$(patsubst lib%.a,-l%,$(LIBS))) \
 		   $(filter -l%,$(patsubst lib%.so,-l%,$(LIBS)))

## BUILD ##############################################################
.PHONY: all
all: $(DEP) $(BIN) 
-include $(DEP)

.PHONY: doc
doc:
	$(MAKE) -C $(DOCDIR)

.PHONY: clean
clean:
	$(RM) $(OBJDIR)/*.o $(LIBDIR)/*.a $(LIBDIR)/*.so
	$(RM) $(SRCDIR)/*~ $(HEADDIR)/*~
	$(RM) $(DEP)
	-$(RMDIR) $(OBJDIR) 2> /dev/null
ifneq ($(BINDIR),.)
	@-$(MAKE) clean -C $(DOCDIR)
endif

.PHONY: distclean
distclean:
	$(RM) $(BINDIR)/$(BIN)
	-$(RMDIR) $(BINDIR) $(LIBDIR) 2> /dev/null

## BUILD ##############################################################
$(BINDIR)/$(BIN): $(OBJ) | $(LIBS) $(BINDIR)
	$(CC) $^ -o $@ $(LDLIBS) $(LDFLAGS)

$(OBJDIR)/%.o: $(addprefix $(SRCDIR)/,%.c)
	$(CC) $(CLIBS) $(CFLAGS) -c $< -o $@

$(CONFDIR)/%.d: $(addprefix $(SRCDIR)/,%.c)
	$(CC) $(CLIBS) -MM $< $(LDLIBS) 1> $*.d
	@$(CP)  $*.d $*.d.tmp
	@$(SED) -e 's/.*:/$(OBJDIR)\/$*.o:/' -i $*.d
	@$(SED) -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | $(FMT) | \
	  $(SED) -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@$(RM) $*.d.tmp

lib%.so: $(SRCDIR)/%.c
	$(CC) -fPIC $(CFLAGS) $(CLIBS) -c $< -o $(OBJDIR)/$*.o
	$(CC) -o $(LIBDIR)/$@ $(SOFLAGS) $(OBJDIR)/$*.o 

lib%.a: $(OBJDIR)/$(notdir %.o)
	$(AR) $(ARFLAGS) $(LIBDIR)/$@ $<

# TESTS ################################################################
%: $(TESTDIR)/%.c
	$(CC) $< $(CFLAGS) $(CLIBS) -o $(BINDIR)/test$* $(LDLIBS) $(LDFLAGS)

# GENERATED DIR ########################################################
ifneq ($(SRCDIR),.)
$(SRCDIR):
	@ echo Creating directory for source "$@"
	-$(MKDIR) $@
endif

ifneq ($(OBJDIR),.)
$(OBJDIR):
	@ echo Creating directory for objects "$@"
	-$(MKDIR) $@
endif

ifneq ($(BINDIR),.)
$(BINDIR):
	@ echo Creating directory for binaries "$@"
	-$(MKDIR) $@
endif

ifneq ($(LIBDIR),.)
$(LIBDIR):
	@ echo Creating directory for libraries "$@"
	-$(MKDIR) $@
endif

ifneq ($(DOCDIR),.)
$(LIBDIR):
	@ echo Creating directory for documents "$@"
	-$(MKDIR) $@
endif

ifneq ($(CONFDIR),.)
$(CONFDIR):
	@ echo Creating directory for config files "$@"
	-$(MKDIR) $@
endif

ifneq ($(TESTDIR),.)
$(TESTDIR):
	@ echo Creating directory for tests "$@"
	-$(MKDIR) $@
endif

ifneq ($(HEADDIR),.)
$(HEADDIR):
	@ echo Creating directory for headers "$@"
	-$(MKDIR) $@
endif
