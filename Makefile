CC:=clang
CCFLAGS:=-Wall -Werror -MMD -MP
BINNAME:=main
SRCDIR:=.

SRC:=$(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c) $(wildcard $(SRCDIR)/*/*/*.c)

OBJ:=$(SRC:.c=.o)

DEP:=$(SRC:.c=.d)

# $@ = name of the current target
# @^ = current targets dependencies
# $@:$^
# adding a build directory is probobly a good idea

all:$(BINNAME)


$(BINNAME):$(OBJ)
	$(CC) $(CCFLAGS) -o $@ $(OBJ) -lm

%.o:%.c Makefile
	$(CC) $(CCFLAGS) -c -o $@ $<

-include $(DEP)

clean:
	@rm -f $(OBJ)
	@rm -f $(DEP)
	@rm -f $(BINNAME)
