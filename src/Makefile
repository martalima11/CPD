serial: maxsat-serial.c
	gcc -o maxsat-serial maxsat-serial.c -Wall -lm -fopenmp

serial_run: serial
	./maxsat-serial ./data/${input}
