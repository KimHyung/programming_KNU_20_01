TARGET = matrix
OBJ= main.o matrix.o
LFLAGS = -pthread -lpthread
CFLAGS = -D_REENTRANT

all: $(OBJ)
	gcc -o $(TARGET) $(LFLAGS) $(OBJ)

main.o: main.c 
	gcc -c $(CFLAGS) main.c

matrix.o: matrix.c
	gcc -c $(CFLAGS) matrix.c

clean:
	rm -r *.o $(TARGET)