# Compiler to use
CC = clang

# Flags to pass compiler
CFLAGS = -fsanitize=signed-integer-overflow -fsanitize=undefined -ggdb3 -O0 -Qunused-arguments -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wshadow

# Name for executable
EXE = race

# Space-separated list of header files
HDRS = helpers.h

# Space-separated list of libraries, if any,
# Each of which should be prefixed with -l
LIBS =

# Space-separated list of source files
SRCS = race.o helpers.c

# Automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# Default target
$(EXE): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# Dependencies
$(OBJS): $(HDRS) Makefile

# Housekeeping
clean:
	rm -f core $(EXE) *.o
