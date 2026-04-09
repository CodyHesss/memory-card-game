CC = gcc
CFLAGS = -Wall -std=c11 -g
TARGET = memory_game

all: $(TARGET)

$(TARGET): memory_game.c
	$(CC) $(CFLAGS) -o $(TARGET) memory_game.c

clean:
	rm -f $(TARGET) save_game.txt
