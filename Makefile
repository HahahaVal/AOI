.PHONY: all clean

CFLAGS ?= -g -O2 -Wall -fPIC -shared

LUA_INC ?= ./lua-5.3.4/src

all:aoi.so

aoi.so:lua_aoi.cpp
	g++ $(CFLAGS) lua_aoi.cpp -o aoi.so -I$(LUA_INC)
clean:
	-rm -rf *.o *.so 