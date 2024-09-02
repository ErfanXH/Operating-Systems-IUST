CC = gcc
CFLAGS = -Wall -g
SRC = Q3.c
OBJ = $(SRC:.c=.o)
TARGET = Matrix

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
