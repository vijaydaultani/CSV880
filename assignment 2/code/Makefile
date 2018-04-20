CC=g++
DEPS= common.h
OBJ = comm.o sim.o test.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $<

a.out: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm *.o a.out
