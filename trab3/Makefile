CC = mpicc          
CFLAGS = -Wall      
LDFLAGS =          
SOURCES = verifica.c chrono.c multipart.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = multipart-mpi

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
