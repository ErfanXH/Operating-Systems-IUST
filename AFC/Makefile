CC = gcc
CFLAGS = -Wall -g
SRC = Q2.c
OBJ = $(SRC:.c=.o)
TARGET = AFC

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
