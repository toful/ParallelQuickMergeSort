######################################################################
#                           Compiladors
#                        Cristòfol Daudén Esmel
#                            Makefile
######################################################################

# General defines
CC = cc

SRC = P1.1_CPM_XX.c

BIN = parallel.obj

CFLAGS = -fopenmp

######################################################################
all :
		$(CC) -O3 $(SRC) $(CFLAGS) -o $(BIN)

clean :
		rm -rf $(BIN)


run : clean all
		./$(BIN) 300000000 2
