EXEC = a.out

CC = gcc
CFLAGS = -Wall
SRC = mainfirst.c functiiStack.c functiiStack.h treeFunctions.c treeFunctions.h 

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

clean:
	rm -f $(EXEC)
