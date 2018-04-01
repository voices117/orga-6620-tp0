# makefile parameters
BIN_NAME    := tp
SRCDIR      := src
TESTDIR     := tests
BUILDDIR    := int
TARGETDIR   := target
SRCEXT      := c

# compiler parameters
CC          := gcc
CFLAGS      := -O3 -std=c99 -Wall -Wpedantic -Werror
LIB         :=
INC         := /usr/local/include
DEFINES     :=


#---------------------------------------------------------------------------------
# DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------

# binaries file names
# each binary source code is expected to be in $(SRCDIR)/<binary>
TARGET := $(TARGETDIR)/$(BIN_NAME)

# sets the src directory in the VPATH
VPATH := $(SRCDIR)

# sets the build directory based on the binary
BUILDDIR := $(BUILDDIR)

# source files
SRCS := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

# object files
OBJS := $(patsubst %,$(BUILDDIR)/$(BIN_NAME)/%,$(SRCS:.$(SRCEXT)=.o))

# includes the flag to generate the dependency files when compiling
CFLAGS += -MD


# special definitions used for the unit tests
ifeq ($(MAKECMDGOALS),tests)
    # adds an extra include so the tests can include the sources
	INC += src

	# sets the special define for tests
	DEFINES := __TESTS__ $(DEFINES)

	# includes the tests directory in the VPATH
	VPATH := $(TESTDIR) $(VPATH)

	# test sources
	TEST_SRCS := $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))

	# test objects
	OBJS := $(patsubst $(BUILDDIR)/$(BIN_NAME)/%,$(BUILDDIR)/tests/%,$(OBJS))
	OBJS := $(patsubst %,$(BUILDDIR)/tests/%,$(TEST_SRCS:.$(SRCEXT)=.o)) $(OBJS)
endif

# adds the include prefix to the include directories
INC := $(addprefix -I,$(INC))

# adds the lib prefix to the libraries
LIB := $(addprefix -l,$(LIB))

# adds the define prefix to the defines
DEFINES := $(addprefix -D,$(DEFINES))


# builds the binary
$(TARGET): $(OBJS) | dirs
	@$(CC) $(CFLAGS) $(INC) $(DEFINES) $^ $(LIB) -o $@
	@echo "LD $@"

# compiles the tests
tests: $(TARGET) | dirs
	python tests/run.py $(TARGET)

# shows usage
help:
	@echo "Para compilar el binario:"
	@echo
	@echo "\t\033[1;92m$$ make\033[0m"
	@echo
	@echo "Para compilar y ejecutar las pruebas (require python 2.7+ instalado):"
	@echo
	@echo "\t\033[1;92m$$ make tests\033[0m"
	@echo
	@echo "Los binarios compilados se encuentran en \033[1;92m$(TARGETDIR)\033[0m."
	@echo

# clean objects and binaries
clean:
	@$(RM) -rf $(BUILDDIR) $(TARGETDIR)

# creates the directories
dirs:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

# rule to build object files
$(BUILDDIR)/$(BIN_NAME)/%.o $(BUILDDIR)/tests/%.o: %.$(SRCEXT)
	@mkdir -p $(basename $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) $(INC) $(DEFINES) $(LIB) -c -o $@ $<


.PHONY: clean dirs tests

# includes generated dependency files
-include $(OBJS:.o=.d)
