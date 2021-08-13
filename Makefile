.PHONY: all clean

CFLAGS ?= -g -O2 -Wall -fPIC -shared

LUA_PATH ?= ./lua-5.4.2
INCLUDE_PATH ?= -I$(LUA_PATH)

all:aoi.so

%.o:%.cpp
	g++ $(CFLAGS) $(INCLUDE_PATH) -o $@ -c $^

aoi.so:lua_aoi.o grid_aoi.o
	g++ $(CFLAGS) $(INCLUDE_PATH) $^ -o $@ 

test: test.o
	g++ -g -O2 -Wall $(INCLUDE_PATH) $^ -o $@

clean:
	-rm -rf *.o *.so