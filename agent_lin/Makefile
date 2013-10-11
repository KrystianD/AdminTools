CXX=g++
OBJS=main.o
CFLAGS=-O0 -static -static-libgcc
LFLAGS=

build/client: $(OBJS)
	mkdir -p build
	$(CXX) $(CFLAGS) $(LFLAGS) $(OBJS) -o $@

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o build/client
