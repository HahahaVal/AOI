.PHONY: all clean

CFLAGS = -fPIC -shared

all: grid_aoi.o

%.o:%.cpp
	g++ $(CFLAGS) -o aoi.so $^

clean:
	-rm -rf *.o *.so 