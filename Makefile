#SSC0903 - Grupo 07
# Eduardo Cavalari Valença 11234381
# Che Fan Pan 11200421
# Marcos Vinícius Firmino Pietrucci 10770072

#Compilador
CC = gcc 

#Flags de compilação
CFLAGS = -fopenmp -I. -g -Wall -O3 -Ofast -march=native

#Bibliotecas usadas
LIBS = -lm

#OBJ= arquivos.c main.o
OBJ_par = jacobipar.c
OBJ_seq = jacobiseq.c

all: 
	@$(CC) -o jacobipar $(OBJ_par) $(CFLAGS) $(LIBS); \
	$(CC) -o jacobiseq $(OBJ_seq) $(CFLAGS) $(LIBS); \

jacobipar: $(OBJ_par)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

jacobiseq: $(OBJ_seq)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run_jacobiseq:
	@read -p "Insira a dimensao da matriz N: " SIZE; \
	./jacobiseq $${SIZE};

run_jacobipar:
	@read -p "Insira a dimensao da matriz N: " SIZE; \
	read -p "Insira a quantidade de threads T: " TRED; \
	./jacobipar $${SIZE} $${TRED};

clean:
	@rm -f *.o jacobiseq jacobipar
