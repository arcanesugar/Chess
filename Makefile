CPP:=clang++
CPPFLAGS:=-Wall -MMD -MP
CC:=clang
CCFLAGS:=-Wall -MMD -MP
BINNAME:=main
SRCDIR:=.

SRC:=$(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp) $(wildcard $(SRCDIR)/*/*/*.cpp)
CSRC:=$(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c) $(wildcard $(SRCDIR)/*/*/*.c)

OBJ:=$(SRC:.cpp=.o) $(CSRC:.c=.o)
DEP:=$(SRC:.cpp=.d) $(CSRC:.c=.d)

# $@ = name of the current target
# @^ = current targets dependencies
# $@:$^
# this makefile does fill up the project with .o files, but I dont want to fix that right now

all:$(BINNAME)

$(BINNAME):$(OBJ)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJ)

%.o:%.cpp Makefile
	$(CPP) $(CPPFLAGS) -c -o $@ $<

%.o:%.c Makefile
	$(CC) $(CCFLAGS) -c -o $@ $<

-include $(DEP)
clean:
	@rm -f $(OBJ)
	@rm -f $(DEP)
	@rm -f $(BINNAME)
