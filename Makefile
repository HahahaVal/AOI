.PHONY: all clean

CFLAGS = -std=c++11 -Wall -fPIC -shared

all:aoi.so

aoi.so:lua_aoi.cpp
	g++ $(CFLAGS) lua_aoi.cpp -o aoi.so -llua

clean:
	-rm -rf *.o *.so 