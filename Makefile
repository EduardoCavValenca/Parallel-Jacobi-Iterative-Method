#Compilador
CC=gcc -fopenmp

#Flags de compilação
CFLAGS=-I. -g -Wall -O3 -Ofast -march=native

#Bibliotecas usadas
LIBS= -lm

#Cabeçalhos
#DEPS=

#OBJ= arquivos.c main.o 
OBJ_par= jacobipar.o
OBJ_seq = jacobiseq.o

%.o: %.c $(DEPS)		
	@$(CC) -c -o $@ $< $(CFLAGS)

jacobipar: $(OBJ_par)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

jacobiseq: $(OBJ_seq)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run_jacobiseq:
	@read -p "Insira a dimensao da matriz N: " SIZE; \
	./jacobiseq $${SIZE};

run_jacobipar:
	@read -p "Insira a dimensao da matriz N: " SIZE; \
	read -p "Insira a quantidade de threads T: " TRED; \
	./jacobipar $${SIZE} $${TRED};

clean:
	@rm -f *.o  jacobiseq

debug:
	gdb ./jacobiseq

#Configurate connection to server
USER = ssc903-ta-g07
PORT = 2210
LASDPC_IP=andromeda.lasdpc.icmc.usp.br
ARQ := password.txt
PSSW := $(shell cat ${ARQ})

.PHONY: connect
connect:
	sshpass -p $(PSSW) ssh $(USER)@$(LASDPC_IP) -p $(PORT)

.PHONY: send_seq
send_seq:
	sshpass -p $(PSSW) scp -P $(PORT) jacobiseq Makefile $(USER)@$(LASDPC_IP):/home/$(USER)/

.PHONY: send_par
send_par:
	sshpass -p $(PSSW) scp -P $(PORT) jacobipar Makefile $(USER)@$(LASDPC_IP):/home/$(USER)/
