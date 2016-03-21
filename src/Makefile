input := "ex1.in"

serial: maxsat-serial.c
	gcc -o maxsat-serial maxsat-serial.c -Wall -lm -fopenmp

serial_run: serial
	./maxsat-serial ./data/${input}

serial_val: serial
	valgrind --track-origins=yes --leak-check=full ./maxsat-serial ./data/${input}
