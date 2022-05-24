#Compilador
CC=gcc -fopenmp

#Flags de compilação
CFLAGS=-I. -g -Wall -O3 -Ofast -march=native

#Bibliotecas usadas
LIBS= -lm

#Cabeçalhos
DEPS= matrix.h vector.h jacobi.h jacobipar.h jacobiseq.h common.h

#OBJ= arquivos.c main.o 
OBJ= main.o matrix.o vector.o jacobi.o jacobipar.o jacobiseq.o

%.o: %.c $(DEPS)		
	@$(CC) -c -o $@ $< $(CFLAGS)

jacobiseq: $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run:
	@./jacobiseq

clean:
	@rm -f *.o  jacobiseq

debug:
	gdb ./jacobiseq