input := "ex1.in"

serial: maxsat-serial.c
	gcc -o maxsat-serial maxsat-serial.c -Wall -lm

serial_run: serial
	./maxsat-serial ./data/${input}

serial_val: serial
	valgrind --track-origins=yes --leak-check=full ./maxsat-serial ./data/${input}

serial_time: serial
	time -f %E ./maxsat-serial ./data/${input}

openmp: maxsat-openmp.c
	gcc -o maxsat-openmp maxsat-openmp.c -Wall -lm -fopenmp

openmp_run: openmp
	./maxsat-openmp ./data/${input}

openmp_val: openmp
	valgrind --tool=helgrind --track-origins=yes --leak-check=full ./maxsat-openmp ./data/${input}

openmp_time: openmp
	time -f %E ./maxsat-openmp ./data/${input}
