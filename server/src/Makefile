CC = gcc
CFLAGS = -Wall -g
TARGET = server
SRC = main.c server.c commands.c arg_parser.c utils.c session.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
