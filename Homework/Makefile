TARGET = matrix
OBJ= main.o matrix.o mat_mul.o mat_mul_th.o mat_mul_th_core.o
LFLAGS = -pthread -lpthread
CFLAGS = -D_REENTRANT

all: $(OBJ)
	gcc -o $(TARGET) $(LFLAGS) $(OBJ)

main.o: main.c 
	gcc -c $(CFLAGS) main.c

mat_mul.o: mat_mul.c
	gcc -c $(CFLAGS) mat_mul.c

mat_mul_th.o: mat_mul_th.c
	gcc -c $(CFLAGS) mat_mul_th.c

mat_mul_th_core.o: mat_mul_th_core.c
	gcc -c $(CFLAGS) mat_mul_th_core.c

matrix.o: matrix.c
	gcc -c $(CFLAGS) matrix.c

clean:
	rm -r *.o $(TARGET)