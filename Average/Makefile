CC = gcc
CFLAGS = -Wall -g
SRC = Q1.c
OBJ = $(SRC:.c=.o)
TARGET = Numbers

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
