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

<<<<<<< HEAD
#Bibliotecas usadas
=======
#jacobipar.c -o jacobipar -fopenmp -lm 
#mpirun -np 4 jacobipar_mpi 3 2

#Bibliotecas usadasclea
>>>>>>> e42d83038f6e444dbfd3a12f6df4b33102f4b568
LIBS = -lm

#OBJ= arquivos.c main.o
OBJ_par = jacobipar.c
OBJ_seq = jacobiseq.c

all: 
<<<<<<< HEAD
	@$(CC) -o jacobipar $(OBJ_par) $(CFLAGS) $(LIBS); \
	$(CC) -o jacobiseq $(OBJ_seq) $(CFLAGS) $(LIBS); \
=======
	@$(MPI) -o jacobipar $(OBJ_par) $(CFLAGS) $(LIBS); \
	$(CC) -o jacobiseq $(OBJ_seq) $(CFLAGS) $(LIBS);
>>>>>>> e42d83038f6e444dbfd3a12f6df4b33102f4b568

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
<<<<<<< HEAD
	@rm -f *.o jacobiseq jacobipar

debug:
	gdb ./jacobiseq
=======
	@rm -f *.o jacobiseq jacobipar;
>>>>>>> e42d83038f6e444dbfd3a12f6df4b33102f4b568

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
<<<<<<< HEAD
	sshpass -p $(PSSW) scp -P $(PORT) jacobipar jacobiseq *.c makefile $(USER)@$(LASDPC_IP):/home/$(USER)/
=======
	sshpass -p $(PSSW) scp -P $(PORT) jacobipar jacobiseq *.c *.txt makefile $(USER)@$(LASDPC_IP):/home/$(USER)/
>>>>>>> e42d83038f6e444dbfd3a12f6df4b33102f4b568

.PHONY: send_seq
send_seq:
	sshpass -p $(PSSW) scp -P $(PORT) jacobiseq makefile $(USER)@$(LASDPC_IP):/home/$(USER)/

.PHONY: send_par
send_par:
	sshpass -p $(PSSW) scp -P $(PORT) jacobipar makefile $(USER)@$(LASDPC_IP):/home/$(USER)/

<<<<<<< HEAD
.PHONY: send_bench
send_bench:
	sshpass -p $(PSSW) scp -P $(PORT) jacobipar makefile $(USER)@$(LASDPC_IP):/home/$(USER)/


.PHONY: download_files
download_files:
	sshpass -p $(PSSW) scp -P $(PORT) $(USER)@$(LASDPC_IP):/home/$(USER)/data/* ./




=======
>>>>>>> e42d83038f6e444dbfd3a12f6df4b33102f4b568
