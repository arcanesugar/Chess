CPP:=clang++
CPPFLAGS:=-Wall
BINNAME:=main
SRCDIR:=.

SRC:=$(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp) $(wildcard $(SRCDIR)/*/*/*.cpp)
OBJ:=$(SRC:.cpp=.o)

# $@ = name of the current target
# @^ = current targets dependencies
# $@:$^
# this makefile does fill up the project with .o files, but I dont want to fix that right now

all:$(BINNAME)

$(BINNAME):$(OBJ)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJ)

%.o:%.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $^

clean:
	@rm -f $(OBJ)
	@rm -f $(BINNAME)
