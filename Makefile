#Compilador
CC=gcc

#Flags de compilação
CFLAGS=-I. -g -Wall

#Bibliotecas usadas
LIBS=-lm

#Cabeçalhos
DEPS= matrix.h vector.h jacobi.h common.h

#OBJ= arquivos.c main.o 
OBJ= main.o matrix.o vector.o jacobi.o

%.o: %.c $(DEPS)		
	@$(CC) -c -o $@ $< $(CFLAGS)

jacobipar: $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run:
	@./jacobipar

clean:
	@rm -f *.o  jacobipar

debug:
	gdb ./jacobipar