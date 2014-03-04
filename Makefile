CC?=g++
CFLAGS=-c -Wall -std=gnu++0x -fPIC -g
LDFLAGS=
LIBS=-lmeta -lboost_system -lboost_thread 
TSTLIBS=-lstdc++ -lpthread
#-lprofiler
LIBFLAGS=-shared
LIBSOURCES=connection.cpp metaext.cpp memstream.cpp runcall.cpp typemap.cpp serialize.cpp
TSTSOURCES=main.cpp testclass.cpp
LIBOBJECTS=$(LIBSOURCES:.cpp=.o)
TSTOBJECTS=$(TSTSOURCES:.cpp=.o)
LIB=libcobject-client.so
TST=cobject-client-test
INSTALL_PATH?=/usr/local
INSTALL_LIBS_DIR=$(INSTALL_PATH)/lib
INSTALL_HEADERS_DIR=$(INSTALL_PATH)/include/cobject-client

all: $(LIBSOURCES) $(LIB)
	
$(LIB): $(LIBOBJECTS) 
	$(CC) $(LDFLAGS) $(LIBFLAGS) -o $@ $(LIBOBJECTS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
tests: $(LIBSOURCES) $(TSTSOURCES) $(TST)
	
$(TST): $(LIBOBJECTS) $(TSTOBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $(LIBOBJECTS) $(TSTOBJECTS) $(LIBS) $(TSTLIBS)
	
install: all $(INSTALL_HEADERS_DIR)
	cp $(LIB) $(INSTALL_LIBS_DIR)
	cp *.hpp $(INSTALL_HEADERS_DIR)
	
$(INSTALL_HEADERS_DIR):
	mkdir $(INSTALL_HEADERS_DIR)

clean: 
	rm -f $(LIB) $(TST) *.o
