#######################################################################
#                             OPTIONS                                 #
#######################################################################

# PROGRAMS #############################################################
AR := ar
CC := gcc
CP := cp -f
RM := rm -f
SED := sed
FMT := fmt -1
CAT := cat
LEX  := flex
YACC := bison
FIND = find $(FDIR) -type d
MAKE += --no-print-directory
MKDIR := mkdir -p
RMDIR := rmdir --ignore-fail-on-non-empty

# DIRECTORIES ##########################################################
# To a multiple directory project, we suggest the names:
# src, obj, bin, lib, doc, conf, test, include, install, data
SRCDIR  := .
OBJDIR  := .
BINDIR  := .
LIBDIR  := .
DOCDIR  := doc
CONFDIR := .
TESTDIR := .
HEADDIR := .
INSTDIR := .
DATADIR := .

-include $(CONFDIR)/directories.mk
VPATH = $(CONFDIR):$(SRCDIR):$(LIBDIR):$(BINDIR):$(TESTDIR):$(HEADDIR)

# SOURCE ###############################################################
BIN := ep4
SRC := $(notdir $(shell ls $(SRCDIR)/*.c))
LIB := $(CONFDIR)/libraries.mk
DEP := $(addprefix $(CONFDIR)/,$(SRC:.c=.d))

-include $(LIB)
OBJ := $(filter-out $(ARLIB) $(SOLIB),$(SRC)) # Tira bibliotecas
OBJ := $(patsubst %.c,%.o,$(OBJ))             # Substitui .c por .o
OBJ := $(addprefix $(OBJDIR)/,$(OBJ))         # Adiciona diretório

# INSTALL CONFIGS ######################################################
# Instalation options (on if 'true', off if 'false')
B_INSTALL = 'false'

ifeq ($(B_INSTALL),'true')
# Options to installation
ICON 	:= 
DESKTOP := 

# Messages
INSTALL_SUCCESS   := "Canoa successfully installed!"
UNINSTALL_MESSAGE := "Removing Canoa..."
endif

# INSTALL ##############################################################
ifeq ($(B_INSTALL),'true')
USER = $(shell whoami)

ifeq ($(USER),root) #global
INSTBIN = /usr/bin
INSTAPP = /usr/share/applications
INSTICO = /usr/share/icons
else 				#local
INSTBIN = $(HOME)/.local/bin
INSTAPP = $(HOME)/.local/share/applications
INSTICO = $(HOME)/.local/share/icons
endif

endif

# COMPILATION ##########################################################
FDIR = $(HEADDIR) # Gerando diretórios
CLIBS  := -I. $(patsubst %,-I%,$(filter-out .%,$(shell $(FIND))))

# Flags para processo de compilação
CFLAGS := -ansi -Wall -pedantic -g

# LINKAGE ##############################################################
FDIR = $(LIBDIR) # Gerando bibliotecas
LDLIBS   = -L. $(patsubst %,-L%,$(filter-out .%,$(shell $(FIND))))

# Flags para processo de ligação
LDFLAGS := -lm
LDFLAGS += -Wl,-rpath,$(LIBDIR)
LDFLAGS += $(filter -l%,$(patsubst lib%.a,-l%,$(LIBS))) \
 		   $(filter -l%,$(patsubst lib%.so,-l%,$(LIBS)))

########################################################################
#                            INSTALLATION                              #
########################################################################

.PHONY: all
all: $(DEP) $(addprefix $(BINDIR)/,$(BIN))
-include $(DEP)

ifeq ($(B_INSTALL), 'true')
.PHONY: install
install:
	@ echo "Configuring executable..."
	@ chmod u+x $(BINDIR)/$(BIN)
	@ $(MKDIR) $(INSTBIN)
	@ $(CP) $(BINDIR)/$(BIN) $(INSTBIN)
	@ echo "Creating desktop icon..."
	@ $(MKDIR) $(INSTAPP)
	@ $(SED) -e 's|\$$(INSTBIN)|$(INSTBIN)|' \
	 	-e 's|\$$(BIN)|$(BIN)|' \
	 	-e 's|\$$(INSTICO)|$(INSTICO)|' \
	 	-e 's|\$$(ICON)|$(ICON)|' \
	 	< $(INSTDIR)/$(DESKTOP) \
	 	> $(INSTAPP)/$(DESKTOP)
	@ $(MKDIR) $(INSTICO)
	@ $(CP) $(INSTDIR)/$(ICON) $(INSTICO)/
	@ echo $(INSTALL_SUCCESS)
endif

.PHONY: doc
doc:
	$(MAKE) -C $(DOCDIR)

########################################################################
#                           SYSTEM CLEAN                               #
########################################################################

.PHONY: clean
clean:
	$(RM) $(OBJDIR)/*.o $(LIBDIR)/*.a $(LIBDIR)/*.so
	$(RM) $(SRCDIR)/*~ $(HEADDIR)/*~
	$(RM) $(DEP)
	-$(RMDIR) $(OBJDIR) 2> /dev/null

.PHONY: distclean
distclean:
	$(RM) $(BINDIR)/$(BIN) $(CONFDIR)/*.d
	-$(RMDIR) $(BINDIR) $(LIBDIR) 2> /dev/null

ifeq ($(B_INSTALL), 'true')
.PHONY: uninstall
uninstall:
	@ echo $(UNINSTALL_MESSAGE)
	@ $(RM) $(INSTAPP)/$(DESKTOP)
	@ $(RM) $(INSTICO)/$(ICON)
	@ $(RM) $(INSTBIN)/$(BIN)
	@ echo "Done."
endif

########################################################################
#                               BUILD                                  #
########################################################################

# EXECUTABLE ###########################################################
$(BINDIR)/$(BIN): $(OBJ) | $(LIBS) $(BINDIR)
	$(CC) $^ -o $@ $(LDLIBS) $(LDFLAGS)

$(OBJDIR)/%.o: $(addprefix $(SRCDIR)/,%.c)
	$(CC) $(CLIBS) $(CFLAGS) -c $< -o $@

$(OBJ): | $(OBJDIR)
$(LIBS): | $(LIBDIR)

# DEPENDENCIES #########################################################
$(CONFDIR)/%.d: $(addprefix $(SRCDIR)/,%.c)
	$(CC) $(CLIBS) -MM $< 1> $@
	@$(CP)  $@ $@.tmp
	@$(SED) -e 's/.*:/$(OBJDIR)\/$*.o:/' -i $@
	@$(SED) -e 's/.*://' -e 's/\\$$//' < $@.tmp | $(FMT) | \
	  $(SED) -e 's/^ *//' -e 's/$$/:/' >> $@
	@$(RM) $@.tmp

# STATIC LIBRARIES #####################################################
lib%.a: $(OBJDIR)/$(notdir %.o)
	$(AR) $(ARFLAGS) $(LIBDIR)/$@ $<

# SHARED LIBRARIES #####################################################
lib%.so: $(SRCDIR)/%.c
	@ #(filter-out .h,$*) 
	$(CC) -fPIC $(CFLAGS) $(CLIBS) -c $< -o $(OBJDIR)/$*.o
	$(CC) -o $(LIBDIR)/$@ $(SOFLAGS) $(OBJDIR)/$*.o 

# TESTS ################################################################
%: $(TESTDIR)/%.c
	$(CC) $< $(CFLAGS) $(CLIBS) -o $(BINDIR)/test$* $(LDLIBS) $(LDFLAGS)


########################################################################
#                            GENERATE DIRS                             #
########################################################################

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

# ifneq ($(DOCDIR),.)
# $(DOCDIR):
# 	@ echo Creating directory for documents "$@"
# 	-$(MKDIR) $@
# endif

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

# ifneq ($(INSTDIR),.)
# $(INSTDIR):
# 	@ echo Creating directory with installation files "$@"
# 	-$(MKDIR) $@
# endif

ifneq ($(DATADIR),.)
$(DATADIR):
	@ echo Creating directory for headers "$@"
	-$(MKDIR) $@
endif
