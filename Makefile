#https://stackoverflow.com/questions/448910/what-is-the-difference-between-the-gnu-makefile-variable-assignments-a

SHAKE_LIB = shake_x64
CC = g++
CPPFLAGS = -g -Wall -I "./src"


libs: SDL2
LDFLAGS = -lGL -lglut -lpng -lz -lm 
LDLIBS += $(foreach library,$(libs),-l$(library))

#Where to place objs
OBJDIR=./tmp/gcc
#https://stackoverflow.com/questions/5178125/how-to-place-object-files-in-separate-subdirectory
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

headers = $(wildcard src/*.h) \
		  $(wildcard src/*.h) \
	 $(wildcard ./src/*.h) \
	
cppsrc = $(wildcard src/*.cpp) \
		$(wildcard src/app/*.cpp) \
		$(wildcard src/base/*.cpp) \
		$(wildcard src/bottle/*.cpp) \
		$(wildcard src/ext/*.cpp) \
		$(wildcard src/gfx/*.cpp) \
		$(wildcard src/math/*.cpp) \
		$(wildcard src/model/*.cpp) \
		$(wildcard src/net/*.cpp) \
		$(wildcard src/world/*.cpp) 
obj = $(cppsrc:.cpp=.o)

all: $(SHAKE_LIB)

#https://stackoverflow.com/questions/46302351/adapting-a-makefile-to-include-sdl2
$(SHAKE_LIB): $(obj)
	$(LINK.cc) $^ -o $@ $(LDLIBS)
#    $(LINK.cc) $(obj) -o $(SHAKE_LIB) $(LDLIBS)
#$(LINK.cc) $^ -o $@ $(LDLIBS)

clean: 
	rm -f $(OBJDIR)*.o *~
