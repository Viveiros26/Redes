CC = gcc
TARGET = servidor
SRCS = servidor_main.c servidor_lib.c
OBJS = $(SRCS:.c=.o)


compiler: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: clean compiler
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
