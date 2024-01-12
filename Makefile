CC = gcc-13
CFLAGS = -fverbose-asm -Wall -Werror -Wpedantic $(shell pkg-config --cflags openssl gtk4)
LIBS = $(shell pkg-config --libs openssl gtk4)

TARGET = f1ntegrity
SOURCES = main.c fparam.c logging.c utility.c graphics.c
OBJ = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJ)