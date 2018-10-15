CC=gcc
CFLAGS=-I.
LIBS=-lm -lSDL2 -lSDL2_image
OBJ=main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
fimg: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
clean:
	rm -rf *.o
.PHONY: clean
