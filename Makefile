CC = gcc-13
CFLAGS = -fverbose-asm -Wall -Werror -Wpedantic $(shell pkg-config --cflags openssl)
LIBS = $(shell pkg-config --libs --cflags openssl)

TARGET = f1ntegrity
SOURCES = main.c fparam.c
OBJ = $(SOURCES:.c=.o)
ASSEMBLY_FILE = $(TARGET).s

all: $(TARGET) $(ASSEMBLY_FILE)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

$(ASSEMBLY_FILE):$(SOURCES)
	$(CC) $(CFLAGS) -S $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(ASSEMBLY_FILE)