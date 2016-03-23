input := "ex1.in"
logfile := "time.log"

all: serial openmp

serial: maxsat-serial.c
	@gcc -o maxsat-serial maxsat-serial.c -Wall -lm -lrt

serial_run: serial
	@./maxsat-serial ./data/${input}

serial_log: serial
	@echo -n "${input} " > ${logfile}
	@./maxsat-serial ./data/${input} > ${logfile}

serial_val: serial
	@echo -n "ex1.in - " > ${logfile}
	@valgrind --track-origins=yes --leak-check=full ./maxsat-serial ./data/${input}

serial_all: serial
	@echo -n "ex1.in: " > ${logfile}
	@./maxsat-serial ./data/ex1.in >> ${logfile}
	@echo -n "ex2.in: " >> ${logfile}
	@./maxsat-serial ./data/ex2.in >> ${logfile}
	@echo -n "ex3.in: " >> ${logfile}
	@./maxsat-serial ./data/ex3.in  >> ${logfile}
	@echo -n "ex4.in: " >> ${logfile}
	@./maxsat-serial ./data/ex4.in  >> ${logfile}
	@echo -n "ex5.in: " >> ${logfile}
	@./maxsat-serial ./data/ex5.in  >> ${logfile}
	@echo "DONE thank you for your patience"

openmp: maxsat-openmp.c
	@gcc -o maxsat-openmp maxsat-openmp.c -Wall -lm -fopenmp -lrt

openmp_run: openmp
	@./maxsat-openmp ./data/${input}

openmp_log: openmp
	@echo -n "${input} " > ${logfile}
	@./maxsat-openmp ./data/${input} > ${logfile}

openmp_val: openmp
	@valgrind -v --tool=helgrind ./maxsat-openmp ./data/${input}

openmp_all: openmp
	@echo -n "ex1.in: " > ${logfile}
	@./maxsat-openmp ./data/ex1.in >> ${logfile}
	@echo -n "ex2.in: " >> ${logfile}
	@./maxsat-openmp ./data/ex2.in >> ${logfile}
	@echo -n "ex3.in: " >> ${logfile}
	@./maxsat-openmp ./data/ex3.in  >> ${logfile}
	@echo -n "ex4.in: " >> ${logfile}
	@./maxsat-openmp ./data/ex4.in  >> ${logfile}
	@echo -n "ex5.in: " >> ${logfile}
	@./maxsat-openmp ./data/ex5.in  >> ${logfile}
	@echo "DONE thank you for your patience"
