CC = gcc
CFLAGS = -Wall -pthread -O2
SOURCES_COMMON = chrono.c
SOURCE_MAIN = multpart.c

TARGET_A = parteA_1k
TARGET_B = parteB_100k

OBJECTS_COMMON = $(SOURCES_COMMON:.c=.o)

all: $(TARGET_B) $(TARGET_A)

a: $(TARGET_A)

b: $(TARGET_B)

$(TARGET_A): $(SOURCE_MAIN) $(OBJECTS_COMMON)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS_COMMON) $(SOURCE_MAIN) -D N_PART=1000

$(TARGET_B): $(SOURCE_MAIN) $(OBJECTS_COMMON)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS_COMMON) $(SOURCE_MAIN) -D N_PART=100000

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS_COMMON) $(TARGET_A) $(TARGET_B)

.PHONY: all clean a b
