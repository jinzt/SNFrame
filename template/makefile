PREFIX?=/home/liwen/IMServer/debug
IMFRAME_PATH=/home/liwen/IMServer/src/IMFrame
CUR_CONFIG=./IMServer.conf
CUR_LOG=./log.conf

INSTALL_BIN=$(PREFIX)/bin
#CONFIG_FILE=$(PREFIX)/config
#TMP_FILE=$(PREFIX)/tmp


include  $(IMFRAME_PATH)/mak.incl
SRC_CPP=$(wildcard ./src/*.cpp) 

app_obj+=$(patsubst %.cpp,%.o,$(SRC_CPP))

IM_BIN:= ./group_server
CFLAGS +=  -I./src 
 
CC:= g++


all: $(IMFRAME_OBJ) $(app_obj)
	$(CC)    $^ -o $(IM_BIN)   $(CFLAGS) $(LIBFLAGS) 
%.o:%.cpp
	$(CC) -c  $(CFLAGS) $^ -o $@

%.o:%.cc
	$(CC) -c  $(CFLAGS) $^ -o $@ 

%.o:%.c
	$(CC) -c  $(CFLAGS) $^ -o $@


include Makefile.dep

clean:
	rm -r ./src/*.o  ./src/core/*.o  ./src/protocol/*.o    ../redis_client/*.o  $(IM_BIN) 

	
dep:
	$(CC) -MM $(SRC_CPP) $(IMFRAME_SRC) > Makefile.dep
install:
	mkdir -p $(PREFIX) $(CONFIG_FILE) $(INSTALL_BIN) $(TMP_FILE)
	mv $(IM_BIN) $(INSTALL_BIN)
	cp $(CUR_CONFIG) $(CONFIG_FILE)
	cp $(CUR_LOG) $(CONFIG_FILE)
