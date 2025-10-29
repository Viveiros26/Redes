CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGET = cliente

SRC = cliente.c

DOWNLOAD_DIR = downloads

compiler: clean $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(DOWNLOAD_DIR)
