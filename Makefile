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

compile: $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run_jacobiseq:
	read ""
	./jacobiseq

run_jacobipar:
	@read -p "Insira a dimensao da matriz N: " SIZE
	@read -p "Insira a quantidade de threads T: " TRED
	./jacobipar $SIZE TRED
clean:
	@rm -f *.o  jacobiseq

debug:
	gdb ./jacobiseq

#Configurate connection to server
USER = ssc903-ta-g07
PSSW = "TwkVZe"
PORT = 2210
LASDPC_IP=andromeda.lasdpc.icmc.usp.br
FILES = jacobi.c

.PHONY: connect
connect:
	sshpass -p $(PSSW) ssh $(USER)@$(LASDPC_IP) -p $(PORT)

.PHONY: send
send:
	sshpass -p $(PSSW) scp -P $(PORT) $(FILES) $(USER)@$(LASDPC_IP):/home/$(USER)/