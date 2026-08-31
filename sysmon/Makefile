CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lncurses

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = sysmon

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean