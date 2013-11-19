CXX=g++
OBJS=main.o ../common/config.o server.o ../common/kutils.o ../common/common.o sensors.o
CFLAGS=-O0 -static -static-libgcc -I../common
LFLAGS=

build/client: $(OBJS)
	mkdir -p build
	$(CXX) $(CFLAGS) $(LFLAGS) $(OBJS) -o $@

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o ../common/*.o build/client
