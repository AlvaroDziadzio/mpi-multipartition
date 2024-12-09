# Configurações
CC = mpicc          # Usar mpicc em vez de gcc
CFLAGS = -Wall      # Flags do compilador
LDFLAGS =           # Flags de linkagem, se necessário
SOURCES = verifica.c chrono.c main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = multi-partition-mpi

# Regra principal
all: $(TARGET)

# Compilação do executável
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

# Compilação dos objetos
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(OBJECTS) $(TARGET)

# Marcando como 'phony'
.PHONY: all clean
