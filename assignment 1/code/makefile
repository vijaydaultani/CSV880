# Compiler
MPICC = mpic++
override CFLAGS += -std=c++11

all: run_me

run_me: run.o
	$(MPICC) $(CFLAGS) -o run run.o

run.o: MPI_Parallel.cpp
	$(MPICC) $(CFLAGS) -o run.o -c MPI_Parallel.cpp

.PHONY: clean
	
clean :
	-rm -f run_me *.o
