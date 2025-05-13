CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = 

TARGET = tinyShell
SOURCES = main.c process.c commands.c utils.c 
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del *.o $(TARGET).exe