CXX=g++
OBJS=main.o client.o agents.o db.o \
		 sqlite3/sqlite3.o sqlite3/md5.o \
		 ../common/common.o ../common/kutils.o ../common/config.o
CFLAGS=-ggdb -I../common -Isqlite3 \
	-O0 -static -static-libgcc \
	-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION
LFLAGS=

build/server: $(OBJS)
	mkdir -p build
	$(CXX) $(CFLAGS) $(LFLAGS) $(OBJS) -o $@

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o ../common/*.o build/server
