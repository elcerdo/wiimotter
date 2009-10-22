BINS=simplescan
LIBS=bluez
CFLAGS=`pkg-config --cflags $(LIBS)`
LDFLAGS=`pkg-config --libs $(LIBS)`

all: $(BINS)

clean:
	$(RM) *.o $(BINS)

simplescan: simplescan.o
