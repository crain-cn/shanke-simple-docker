CC = gcc
CFLAGS = 

TARGET = skdocker 
CONTAINER_DIR = containers

SRCS = skdocker.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: hello

busybox: $(TARGET)
	./$(TARGET) run busybox /bin/sh

hello: $(TARGET)
	./$(TARGET) run hello ./hello

clean:
	rm -rf a.out $(TARGET) $(OBJS) $(CONTAINER_DIR)/*

