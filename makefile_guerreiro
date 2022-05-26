# Name of the project
PROJ_NAME=jacobiseq jacobipar

# Compiler
CC=gcc

# default run program
PROG = -1

# Flags for compiler
CC_FLAGS=-Wall					\
		 -Wextra				\
		 -Wshadow				\
		 -Wundef				\
		 -Wformat=2				\
		 -Wfloat-equal			\
		 -Wcast-align			\
		 -std=c17				\
		 -march=native			\
		 -O3					\
		 -Ofast

# Libraries
LIBS=-lm -fopenmp

# Compilation
all: $(PROJ_NAME)
	@echo "Done"

$(PROJ_NAME):
	@echo "Compiling $@"
	@$(CC) -o $@ $@.c $(CC_FLAGS) $(LIBS)
	
.PHONY: run
run:
	@if [ $(PROG) = jacobiseq ]; then\
        read -p "Enter the matrix's order: " ORDER \
		&& ./$(PROG) $${ORDER};\
	elif [ $(PROG) = jacobipar ]; then\
        read -p "Enter the matrix's order and the desired number of threads: " ORDER \
		&& ./$(PROG) $${ORDER};\
	else \
        echo "Usage: make run PROG=<prog>, where <prog> can be either jacobiseq or jacobipar.";\
	fi

.PHONY: clean

clean:
	@echo "Cleaning"
	@rm -rf $(PROJ_NAME) *~