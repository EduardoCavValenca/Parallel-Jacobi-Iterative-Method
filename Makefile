#SSC0903 - Grupo 07
# Eduardo Cavalari Valença 11234381
# Che Fan Pan 11200421
# Marcos Vinícius Firmino Pietrucci 10770072

#Compilador
GCC = gcc
MPI = mpicc 
MPRUN = mpirun

#Flags de compilação
CFLAGS = -fopenmp -I. -g -Wall -O3 -Ofast -march=native

#jacobipar.c -o jacobipar -fopenmp -lm 
#mpirun -np 4 jacobipar_mpi 3 2

#Bibliotecas usadasclea
LIBS = -lm

#OBJ= arquivos.c main.o
OBJ_par = jacobipar.c
OBJ_seq = jacobiseq.c

all: 
	@$(MPI) -o jacobipar $(OBJ_par) $(CFLAGS) $(LIBS); \
	$(CC) -o jacobiseq $(OBJ_seq) $(CFLAGS) $(LIBS);

jacobipar: $(OBJ_par)
	$(MPRUN) -np $${PROC} --hostfile halley.txt jacobipar $${SIZE} $${TRED};
	@$(MPI) -o $@ $^ $(CFLAGS) $(LIBS);

jacobiseq: $(OBJ_seq)
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS);

run_jacobiseq:
	@read -p "Insira a dimensao da matriz N: " SIZE; \
	./jacobiseq $${SIZE};

run_jacobipar:
	@read -p "Insira a dimensao da matriz N: " SIZE; \
	read -p "Insira a quantidade de processos P: " PROC;\
	read -p "Insira a quantidade de threads T: " TRED; \
	read -p "Deseja realizar um 'oversubscribe'? (S/N): " OVER; \
	if [ $${OVER} = "S" ]; then\
		$(MPRUN) --oversubscribe -np $${PROC} jacobipar $${SIZE} $${TRED};\
    else \
		$(MPRUN) -np $${PROC} jacobipar $${SIZE} $${TRED};\
	fi

run_cluster:
	@read -p "Insira a dimensao da matriz N: " SIZE; \
	read -p "Insira a quantidade de processos P: " PROC;\
	read -p "Insira a quantidade de threads T: " TRED; \
	read -p "Deseja realizar um 'oversubscribe'? (S/N): " OVER; \
	if [ $${OVER} = "S" ]; then\
		$(MPRUN) --oversubscribe -np $${PROC} jacobipar $${SIZE} $${TRED};\
    else \
		$(MPRUN) -np $${PROC} --hostfile halley.txt jacobipar $${SIZE} $${TRED};\
	fi

clean:
	@rm -f *.o jacobiseq jacobipar;

#Configurate connection to server
USER = ssc903-ta-g07
PORT = 2210
LASDPC_IP=andromeda.lasdpc.icmc.usp.br
ARQ := password.txt
PSSW := $(shell cat ${ARQ})

.PHONY: connect
connect:
	sshpass -p $(PSSW) ssh $(USER)@$(LASDPC_IP) -p $(PORT)

.PHONY: send_all
send_all:
	sshpass -p $(PSSW) scp -P $(PORT) jacobipar jacobiseq *.c *.txt *.sh makefile $(USER)@$(LASDPC_IP):/home/$(USER)/

.PHONY: send_seq
send_seq:
	sshpass -p $(PSSW) scp -P $(PORT) jacobiseq makefile $(USER)@$(LASDPC_IP):/home/$(USER)/

.PHONY: send_par
send_par:
	sshpass -p $(PSSW) scp -P $(PORT) jacobipar makefile $(USER)@$(LASDPC_IP):/home/$(USER)/

.PHONY: download
download:
	sshpass -p $(PSSW) scp -r -P$(PORT) $(USER)@$(LASDPC_IP):/home/$(USER)/paralelo/ ./
