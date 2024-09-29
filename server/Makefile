CC = gcc
CFLAGS = -Wall -g
TARGET = server
SRC = src/main.c src/server.c src/commands.c src/arg_parser.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
