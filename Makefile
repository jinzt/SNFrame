PREFIX?=/home/liwen/IMServer/debug
IMFRAME_PATH=.
CUR_CONFIG=./IMFrame.conf
CUR_LOG=./log.conf

INSTALL_BIN=$(PREFIX)/bin
CONFIG_FILE=$(PREFIX)/config
TMP_FILE=$(PREFIX)/tmp

include  $(IMFRAME_PATH)/mak.incl

IM_BIN:= ./IMFrame
  
CC:= g++


all:$(IMFRAME_OBJ)     
	$(CC)    $^ -o $(IM_BIN)   $(CFLAGS) $(LIBFLAGS) 
%.o:%.cpp
	$(CC) -c  $(CFLAGS) $^ -o $@

%.o:%.cc
	$(CC) -c  $(CFLAGS) $^ -o $@ 

%.o:%.c
	$(CC) -c  $(CFLAGS) $^ -o $@


include Makefile.dep

clean:
	rm -r ./src/base/*.o  ./src/core/*.o  ./src/app/*.o    ../redis_client/*.o  $(IM_BIN) 

	
dep:
	$(CC) -MM $(IMFRAME_SRC)  > Makefile.dep
install:
	mkdir -p $(PREFIX) $(CONFIG_FILE) $(INSTALL_BIN) $(TMP_FILE)
	mv $(IM_BIN) $(INSTALL_BIN)
	cp $(CUR_CONFIG) $(CONFIG_FILE)
	cp $(CUR_LOG) $(CONFIG_FILE)
