CC = gcc
CFLAGS = -Wall -g
SRC = Q4.c
OBJ = $(SRC:.c=.o)
TARGET = Resource

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
