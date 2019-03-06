######################################################################
#                           Compiladors
#                        Cristòfol Daudén Esmel
#                            Makefile
######################################################################

# General defines
CC = cc

SRC = P1.1_CPM_XX.c

BIN = parallel.obj

CFLAGS = -fopenmp -o

######################################################################
all :
		$(CC) -O3 $(SRC) $(CFLAGS) $(BIN)

clean :
		rm -rf $(BIN)


run : clean all
	

test: clean all