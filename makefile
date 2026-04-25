CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = validator
SRC = validator.c

all : $(TARGET)

$(TARGET) : $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run:
	./$(TARGET) $(FILE)

clean:
	rm -f $(TARGET)