.PHONY: all clean

CFLAGS = -std=c++11 -Wall -fPIC -shared -llua

all: grid_aoi.o

%.o:%.cpp
	g++ $(CFLAGS) -o aoi.so $^

clean:
	-rm -rf *.o *.so 