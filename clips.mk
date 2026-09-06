# The makefile CLIPSraylib builds CLIPS with.
#
# The top-level makefile copies this file into the build directory of the
# selected CLIPS version, vendor/clips-build/<tag>, over the makefile that
# CLIPS ships, after it has copied the sources this project adds. It is never
# edited there: that directory is a copy of a downloaded tree, and this file in
# it is replaced on every build. Edit clips.mk in the root of this project.
#
# It replaces the CLIPS makefile rather than adding to it because the object
# list here is computed and not written out. CLIPS 6.4.2, branches/64x and
# branches/70x have different files, and in every one of them the library is
# every .c file except main.c and the files this project adds. Reading the
# directory is therefore both shorter and correct for all three.
#
# The CLIPS sources themselves are compiled exactly as the CLIPS makefile
# compiles them. Only the four files this project adds get anything else:
#
#   userfunctions.c      the UDF handlers; sees clips.h and raylib_glue.h only
#   raylib_glue.c        the raylib side; sees raylib.h and raygui.h, never
#                        clips.h, and is compiled with the same renames as
#                        raylib itself
#   raylib_standalone.c  the UserFunctions CLIPS calls at startup
#
# The variables the top-level makefile passes in:
#
#   BINARY              where to write the clips executable
#   CLIPS_OS            LINUX or DARWIN, which CLIPS wants as a -D
#   RAYLIB_SRC          the raylib src/ directory, for raylib.h and rlgl.h
#   RAYLIB_LIB          the raylib static archive
#   RAYLIB_LDLIBS       that archive plus the system libraries it needs
#   RAYLIB_GLUE_CFLAGS  the rename defines, and any extra include paths

PLATFORM = $(shell uname -s)

ifeq ($(PLATFORM),Darwin) # macOS
	CLIPS_OS ?= DARWIN
	WARNINGS = -Wall -Wundef -Wpointer-arith -Wshadow -Wstrict-aliasing \
	           -Winline -Wmissing-declarations -Wredundant-decls \
	           -Wmissing-prototypes -Wnested-externs -Wstrict-prototypes \
	           -Waggregate-return -Wno-implicit
endif

ifeq ($(PLATFORM),Linux) # linux
	CLIPS_OS ?= LINUX
	WARNINGS = -Wall -Wundef -Wpointer-arith -Wshadow -Wstrict-aliasing \
               -Winline -Wredundant-decls -Waggregate-return
endif

CLIPS_OS ?= LINUX

# Where the executable goes. This file runs in vendor/clips-build/<tag>, three
# levels below the root of the project. The top-level makefile always passes
# an absolute BINARY; this default is for running make in here by hand.
BINARY ?= ../../../clips

RAYLIB_SRC         ?=
RAYLIB_LIB         ?=
RAYLIB_LDLIBS      ?= $(RAYLIB_LIB)
RAYLIB_GLUE_CFLAGS ?=

# Every .c file in the directory is part of the library, except these. main.c
# builds the executable around the library, and the other three are this
# project's, linked as objects beside it rather than archived into it, so
# that libclips.a is the CLIPS core and nothing else.
GLUE_SRCS = userfunctions.c raylib_glue.c raylib_standalone.c
GLUE_OBJS = $(GLUE_SRCS:.c=.o)
NOT_LIB   = main.c $(GLUE_SRCS)

OBJS = $(patsubst %.c,%.o,$(filter-out $(NOT_LIB),$(wildcard *.c)))

all: release

debug : CC = gcc
debug : CFLAGS = -std=c99 -O0 -g
debug : LDLIBS = -lm
debug : clips

release : CC = gcc
release : CFLAGS = -std=c99 -O3 -fno-strict-aliasing
release : LDLIBS = -lm
release : clips

# The rule CLIPS ships, plus -MMD so a header change is noticed, since the
# per-object header lists of the CLIPS makefile are not carried over.
.c.o :
	$(CC) -c -MMD -D$(CLIPS_OS) $(CFLAGS) $(WARNINGS) $<

# raylib_glue.c is the one translation unit that sees raylib's headers. It
# takes the same defines raylib was built with, so that the declaration of a
# renamed function matches its definition in the archive. raygui's functions
# are made static there (see the top of raylib_glue.c), and the many it does
# not call would each draw an unused-function warning under -Wall.
raylib_glue.o : raylib_glue.c raylib_glue.h raygui.h
	$(CC) -c -MMD -D$(CLIPS_OS) $(CFLAGS) -Wall -Wno-unused-function \
	    -I"$(RAYLIB_SRC)" $(RAYLIB_GLUE_CFLAGS) raylib_glue.c

clips : $(BINARY)

# The objects come first and the archives after them, so that every reference
# from a wrapper into the CLIPS core or into raylib is still open when the
# archive that satisfies it is read.
$(BINARY) : main.o $(GLUE_OBJS) libclips.a $(RAYLIB_LIB)
	$(CC) -o "$(BINARY)" main.o $(GLUE_OBJS) -L. -lclips $(RAYLIB_LDLIBS) $(LDLIBS)

libclips.a : $(OBJS)
	rm -f $@
	ar cq $@ $(OBJS)

clean :
	-rm -f *.o
	-rm -f "$(BINARY)" libclips.a
	-rm -f *.d

# The final value of any variable: "make print-OBJS".
print-% :
	@echo '$($*)'

.PHONY : all clips clean debug release

-include $(OBJS:.o=.d) main.d $(GLUE_OBJS:.o=.d)
