CC = gcc
CFLAGS = -Wall
SOURCES = client.c
TARGET = client

default: $(TARGET)
all: default

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	@rm -f $(TARGET) $(TARGET).o
