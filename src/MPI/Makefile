######################################################################
#                            Makefile
######################################################################

# General defines
CC = mpicc

SRC = P2.1_CPM_ElAzizi_Dauden.c

BIN = parallel.obj

CFLAGS = -fopenmp

######################################################################
all :
		$(CC) -Wall -O3 $(SRC) -o $(BIN)

clean :
		rm -rf $(BIN)


run : clean all
		mpirun -c 4 ./$(BIN) 300000000 4
