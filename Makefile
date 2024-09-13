CC = cc
CFLAGS = -I/usr/include/libevdev-1.0/ -ludev -linput -levdev -lpthread
LIBS = -I/usr/include/libevdev-1.0/ -ludev -linput -levdev -lpthread
SRC = uexevents.c
OBJ = $(SRC:.c=.o)
EXE = uexevents

all: $(EXE)
uexevents: uexevents.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
debug:
	$(CC) $(CFLAGS) -g $(SRC) -o debug $(LIBS)
test:
	./$(EXE)
clean:
	rm -f $(OBJ) $(EXE) debug
